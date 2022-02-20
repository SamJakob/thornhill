#include <algorithm>
#include <cmath>
#include <thornhill>

#include "physical.hpp"
#include "kernel/arch/x86_64/include.hpp"

using namespace std;
using namespace Thornhill;

namespace ThornhillMemory {

    /**
     * @brief Returns the first available page with a base address no lower
     * than the specified minimum base address.
     *
     * Returns null (0) if the specified page couldn't be found.
     *
     * @param bootMap The boot-time memory map that is passed into the PMM
     * initialization method.
     * @param minBaseAddress The lowest base address of a page that should
     * be returned.
     * @return The base address of the page with the lowest memory address
     * still meeting the specified criteria.
     */
    static uint64_t _bootMapFirstAvailablePage(HandoffMemoryMap& bootMap, uint64_t minBaseAddress = 1) {
        assert(minBaseAddress > 0);
        uint64_t baseAddress = null;

        for (uintptr_t segmentIndex = 0; segmentIndex < bootMap.mapSize; segmentIndex++) {
            if (bootMap.segments[segmentIndex].memoryType == THAvailableMemory &&
                bootMap.segments[segmentIndex].pageCount >= 1 &&
                bootMap.segments[segmentIndex].physicalBaseAddress >= minBaseAddress) {

                // Grab the smallest non-zero base address.
                if (baseAddress == null || baseAddress > bootMap.segments[segmentIndex].physicalBaseAddress) {
                    baseAddress = bootMap.segments[segmentIndex].physicalBaseAddress;
                }

            }
        }

        return baseAddress;
    }

    static uint64_t _bootMapLastAvailablePage(HandoffMemoryMap& bootMap) {
        uint64_t baseAddress = null;

        for (uintptr_t segmentIndex = 0; segmentIndex < bootMap.mapSize; segmentIndex++) {
            if (bootMap.segments[segmentIndex].memoryType == THAvailableMemory &&
                bootMap.segments[segmentIndex].pageCount >= 1 &&
                bootMap.segments[segmentIndex].physicalBaseAddress >= 1) {

                // Grab the largest base address of that segment's pages.
                uint64_t largestBaseAddress = bootMap.segments[segmentIndex].physicalBaseAddress;

                if (largestBaseAddress > baseAddress) {
                    baseAddress = largestBaseAddress;
                }

            }
        }

        return baseAddress;
    }

    /**
     * Returns the boot map's segment index of the segment of the page that the specified address
     * belongs to.
     *
     * Panics if the segment index couldn't be found.
     *
     * @param bootMap The boot-time memory map that is passed into the PMM
     * initialization method.
     * @param address The address that the segment should contain.
     * @return The integer memory segment ID in bootMap that the address belongs to.
     */
    static uint64_t _bootMapSegmentForPageAddress(HandoffMemoryMap& bootMap, uint64_t address) {
        for (uintptr_t segmentIndex = 0; segmentIndex < bootMap.mapSize; segmentIndex++) {
            if (
                bootMap.segments[segmentIndex].physicalBaseAddress >= address &&
                address < (bootMap.segments[segmentIndex].physicalBaseAddress + PAGES(bootMap.segments[segmentIndex].pageCount))
            ) {
                return segmentIndex;
            }
        }

        Kernel::panic("There was a problem interpreting data from the firmware.");
    }

    bool Physical::initialized = false;
    size_t Physical::usedMemory = 0;
    size_t Physical::totalMemory = 0;
    size_t Physical::inventoryBase = 0;

    void Physical::reset() {
        Physical::initialized = false;
        Physical::usedMemory = 0;
        Physical::totalMemory = 0;
        Physical::inventoryBase = 0;
    }

    void Physical::initialize(HandoffMemoryMap bootMap) {
        Kernel::debug("PMM", "Initializing physical memory manager...");

        if (Physical::initialized) {
            Kernel::panic("Attempted to re-initialize PMM in initialized state.");
        }

        // Take inventory.
        {
            Kernel::debug("PMM", "Taking inventory of memory structures...");

            // Locate the first available page.
            uint64_t firstPage = _bootMapFirstAvailablePage(bootMap);
            // ...and locate the last available page.
            uint64_t lastPage = _bootMapLastAvailablePage(bootMap);

            if (firstPage == null) {
                // If we failed to find a page (indicated by firstPage being null), we should panic
                // because this means there's absolutely no pages available, meaning there's no
                // available system memory at all.
                Kernel::panic("No available memory.");
            }

            // Take the first page for PMM inventory usage and set inventoryBase to its
            // base address.
            Kernel::debugf("PMM", "Taking page (base = %x) for PMM usage.", firstPage);
            Physical::inventoryBase = firstPage;
            assert(Physical::inventoryBase != null);

            ThornhillPhysicalFrame* previousFrame = nullptr;
            // Take the first page as the inventory page.
            uint64_t inventoryPage = firstPage;
            // ...and initialize the current frame to be the first possible frame in the inventory
            // page.
            auto* currentFrame = (ThornhillPhysicalFrame*)inventoryPage;
            // Now, initialize the current page as the next available page after the inventory page.
            uint64_t currentPage = _bootMapFirstAvailablePage(bootMap, inventoryPage + PAGES(1));

            while (currentPage <= lastPage) {

                if (currentPage == null) {
                    Kernel::debug("PMM", "    No more usable pages.");
                    break;
                }

                // Load the firmware information about the current page.
                uint64_t currentSegmentIndex = _bootMapSegmentForPageAddress(bootMap, currentPage);
                HandoffMemorySegment* segment = &bootMap.segments[currentSegmentIndex];
                // If we took the page straight after the inventory base, make sure we update the
                // physicalBaseAddress to reflect the page we're now on.
                if (currentPage == Physical::inventoryBase + PAGES(1)) {
                    segment->physicalBaseAddress = currentPage;
                }
                if (segment->physicalBaseAddress == inventoryPage) {
                    segment->physicalBaseAddress += PAGES(1);
                }

                // Initialize the frame.
                currentFrame->base = currentPage; // segment.physicalBaseAddress;
                currentFrame->metadata = 0;
                currentFrame->next = null;

                // Calculate the size of the bitmap (in bytes) required to hold this contiguous
                // section of pages.
                uint64_t maxPossibleBitmapSize = ((uint64_t) inventoryPage + PAGES(1)) - ((uint64_t)currentFrame + PHYSICAL_FRAME_HEADER_SIZE);
                uint16_t bitmapSize = max(
                    min(
                        // The number of pages this frame holds (divided by 8, because this is a
                        // byte array, where each bit of the byte represents a page).
                        (uint64_t)llceiln((long double)segment->pageCount / 8, 8),
                        // Get the base address of the next consecutive page with PAGE_ALIGN_UP,
                        // and subtract from it the base address of the bitmap. This yields the
                        // maximum possible size of the bitmap.
                        maxPossibleBitmapSize
                    ),
                    0UL
                );

                // If we don't have enough room to make a new frame, simply move on to the next
                // inventory page.
                if (((uint64_t) inventoryPage + PAGES(1)) - ((uint64_t)currentFrame) <= PHYSICAL_FRAME_HEADER_SIZE) {
                    Kernel::debugf("PMM", "Not enough room in inventory for new frame. Skipping...");

                    // Take the next available page as an inventory page.
                    inventoryPage = _bootMapFirstAvailablePage(bootMap, currentPage + PAGES(1));

                    // ...and reduce one page from the segment to reflect that we took a page to
                    // use as an inventory page.
                    bootMap.segments[_bootMapSegmentForPageAddress(bootMap, inventoryPage)].pageCount--;

                    currentFrame = reinterpret_cast<ThornhillPhysicalFrame*>(inventoryPage);

                    // If an inventory page couldn't be found, break early as we've exhausted
                    // available memory.
                    if (inventoryPage == null)
                        break;

                    // Take the next page after the inventory page as the next page.
                    currentPage = _bootMapFirstAvailablePage(bootMap, inventoryPage + PAGES(1));

                    // If a page after the inventory page couldn't be found, likewise break early
                    // as we've evidently exhausted available memory.
                    if (currentPage == null)
                        break;

                    // Otherwise, make that page the base address for its segment, for when we
                    // continue taking inventory.
                    bootMap.segments[_bootMapSegmentForPageAddress(bootMap, currentPage)].physicalBaseAddress = currentPage;

                    continue;
                }

                // If decreasing the pageCount would cause the pageCount to become negative, we
                // know that the last entry of our bitmap needs some bits set to USED (because
                // we didn't have a number of pages divisible by 8).
                auto pageCount = (int64_t) segment->pageCount;
                uint8_t offset = (pageCount - bitmapSize * 8 < 0) ? (bitmapSize * 8) - pageCount : 0;
                // TODO: set remainder to USED.

                // Subtract the page count.
                segment->pageCount -= (bitmapSize * 8) - offset;
                segment->physicalBaseAddress += PAGES(bitmapSize * 8);

                // Load the bitmap base address (which is the current frame offset by the page
                // size.)
                auto* bitmap = (uint8_t*)((uint64_t) currentFrame + PHYSICAL_FRAME_HEADER_SIZE);
                // Zero the bitmap.
                memzero(bitmap, bitmapSize);

                // Determine whether the current bitmap will cause the inventory page to be filled.
                bool willFillInventoryPage = (uint64_t) bitmap + bitmapSize == (uint64_t) inventoryPage + PAGES(1);
//                Kernel::debugf("PMM", "    willFillInventoryPage = %d (%x, %x)",
//                               willFillInventoryPage ? 1 : 0,
//                               ((uint64_t) bitmap) + bitmapSize,
//                               (uint64_t) inventoryPage + PAGES(1));

                // Update the current frame to reflect the newly defined bitmap.
                currentFrame->count = (bitmapSize * 8) - offset;

                // Set the bitmap in the frame.
                currentFrame->bitmap = bitmap;

//                Kernel::debugf("PMM", "    (inventory: %x) Adding %d page(s) (segment = %d, base = %x).",
//                               inventoryPage,
//                               currentFrame->count,
//                               currentSegmentIndex,
//                               currentFrame->base);
                currentPage += PAGES(currentFrame->count);

                // Reset the current frame, update the previous frame if it exists and then replace
                // the 'previousFrame' reference with the current frame.
                if (previousFrame != nullptr)
                    previousFrame->next = reinterpret_cast<uint64_t>(currentFrame);
                previousFrame = currentFrame;

                // If the inventory page was filled, we need to select a new inventory page to
                // continue taking inventory.
                if (willFillInventoryPage && segment->pageCount >= 2) {
                    // Take the next available page as an inventory page.
                    inventoryPage = _bootMapFirstAvailablePage(bootMap, currentPage);
                    currentFrame = reinterpret_cast<ThornhillPhysicalFrame*>(inventoryPage);

                    // If an inventory page couldn't be found, break early as we've exhausted
                    // available memory.
                    if (inventoryPage == null)
                        break;

                    segment->pageCount--;
                    segment->physicalBaseAddress += PAGES(1);

                    currentPage = _bootMapFirstAvailablePage(bootMap, inventoryPage + PAGES(1));
                }
                // Otherwise, offset the inventory page.
                else {
                    currentFrame =
                        reinterpret_cast<ThornhillPhysicalFrame*>((uint64_t) bitmap + bitmapSize);
                    currentPage = _bootMapFirstAvailablePage(bootMap, currentPage + PAGES(1));

                    // TODO: check if this is ANY inventoryPage, rather than just the current one
                    if (currentPage == inventoryPage) {
                        currentPage = _bootMapFirstAvailablePage(bootMap, currentPage + PAGES(1));
                    }

                    segment->physicalBaseAddress = currentPage;
                    assert(segment->physicalBaseAddress != inventoryPage);
                }
            }
        }

        // Check inventory.
        {
            Kernel::debug("PMM", "Checking inventory...");
            auto* currentFrame = reinterpret_cast<ThornhillPhysicalFrame*>(Physical::inventoryBase);

            while (currentFrame != nullptr) {
                Physical::totalMemory += PAGES(currentFrame->count);
                currentFrame = reinterpret_cast<ThornhillPhysicalFrame*>(currentFrame->next);
            }
        }

        if (Physical::totalMemory < 1) {
            Kernel::panic(
                "Out of Memory",
                SYSTEM_SELF_PANIC_INTERRUPT_NUMBER,
                "Physical memory manager was unable to find any available physical memory pages."
            );
        }

        Physical::initialized = true;
        Kernel::debug("PMM", "PMM is ready.");
        Kernel::debugf("PMM", "    Physical memory: %u MiB (%u KiB, %u bytes)", Physical::totalMemory / 1024 / 1024, Physical::totalMemory / 1024, Physical::totalMemory);
    }

    void* Physical::allocate(size_t pageCount) {
        Kernel::debugf("PMM", "Allocating %u pages...", pageCount);
        auto* currentFrame = reinterpret_cast<ThornhillPhysicalFrame*>(Physical::inventoryBase);

        while (currentFrame != null) {
            // The base of the current frame.
            uint64_t base = currentFrame->base;
            // The offset from [base] that the contiguous chunk of memory starts.
            uint64_t baseOffset = 0;
            // The number of presently found contiguous pages after baseOffset.
            uint64_t contiguous = 0;

            for (uint64_t pageGroup = 0; pageGroup < currentFrame->count; pageGroup++) {
                if (contiguous >= pageCount) {
                    for (uint64_t i = 0; i < pageCount; i++) {
                        // Get the bitmap entry based on the floored-by-8 page index.
                        uint64_t bitmapGroupBase = ((baseOffset / TH_ARCH_PAGE_SIZE) + i) / 8;
                        uint64_t bitmapGroupOffset = ((baseOffset / TH_ARCH_PAGE_SIZE) + i) % 8;

                        currentFrame->bitmap[bitmapGroupBase] |= 1 << (7 - bitmapGroupOffset);
                        usedMemory += PAGES(1);
                    }

                    Kernel::debugf("PMM", "Allocated %u pages. (Total: %u, Used Mem: %u, Base: 0x%x)", pageCount, usedMemory / TH_ARCH_PAGE_SIZE, usedMemory, base + baseOffset);
                    return (void*) (base + baseOffset);
                }

                // If the entire group in the bitmap is free, then mark the entire group as
                // free contiguous pages.
                if (currentFrame->bitmap[pageGroup] == 0) contiguous += 8;
                // Otherwise, there is an entry in the bitmap that is not free, so we need to
                // start counting from the first free page.
                else {
                    // Figure out the 0-indexed location of the first free page in this group.
                    uint8_t location = 0;

                    for (uint8_t check = 8; check <= 8; check--) {
                        if ((currentFrame->bitmap[pageGroup] & (0b1 << check)) > 0) {
                            location = 8 - check;
                        }
                    }

                    // If there is no free page in this bitmap group, skip it and continue to
                    // the next one.
                    assert(location != 0);
                    if (location >= 8) {
                        contiguous = 0;
                        baseOffset = PAGES((pageGroup + 1) * 8);
                        continue;
                    }

                    // Now add contiguous pages and set baseOffset.
                    baseOffset = PAGES((pageGroup * 8) + location);
                    contiguous = 8 - location;
                }
            }

            currentFrame = reinterpret_cast<ThornhillPhysicalFrame*>(currentFrame->next);
        }

        Kernel::debugf("PMM", "Failed to allocate %u pages.", pageCount);
        return nullptr;
    }

    [[maybe_unused]] void Physical::deallocate([[maybe_unused]] void* base, [[maybe_unused]] size_t pageCount) {
        Kernel::debugf("PMM", "Deallocating %u pages, (base address 0x%x)...", pageCount, base);
        auto* currentFrame = reinterpret_cast<ThornhillPhysicalFrame*>(Physical::inventoryBase);

        // Simple sanity check: ensure deallocating pageCount would not free more memory than was
        // already in use.
        if (PAGES(pageCount) > usedMemory) {
            Kernel::panic("Attempted to free more memory than in use.");
        }

        // Save pointers as integer for easier arithmetic.
        auto baseAddress = (uint64_t) base;

        // Find the frame that the contiguous chunk of memory starts at.
        while (currentFrame != null) {
            // When we've found the frame exit the loop.
            if (currentFrame->base >= baseAddress) break;

            // Otherwise, keep looking.
            currentFrame = reinterpret_cast<ThornhillPhysicalFrame*>(currentFrame->next);
        }

        if (currentFrame == null) {
            Kernel::panic("Tried to deallocate non-existent memory. (1)");
        }

        // Floor the base address to its nearest aligned value.
        auto bitmapEntry = ((baseAddress - (uint64_t) currentFrame->base) / 8);

        // The offset from the base address (from 0 to 7) of the base of this contiguous chunk
        // of pages.
        uint8_t baseOffset = (baseAddress - (uint64_t) currentFrame->base) % 8;

        // Special case: if the page count is less than 8 AND we can do all work in one bitmask
        // operation, then do so and simply return straight away.
        if (pageCount + baseOffset < 8) {
            uint8_t mask = 0;

            for (uint8_t i = baseOffset; i < pageCount + baseOffset; i++) {
                mask |= 1 << i;
            }

            mask = ~mask;
            currentFrame->bitmap[bitmapEntry] &= mask;

            usedMemory -= PAGES(pageCount);
            return;
        }

        // If the above is not zero, we deallocate the offset pages and increment 'bitmapEntry' to
        // the base of the allocation bit of the next aligned page.
        if (baseOffset > 0) {
            // Create a bitmask, preserving only the bits leading up to the offset.
            uint8_t mask = 0;
            for (uint8_t i = 0; i < baseOffset; i++) {
                mask |= 1 << i;
            }

            // Then bitwise AND the bitmap value by the created mask.
            currentFrame->bitmap[bitmapEntry] &= mask;

            // Update our 'bitmapEntry' value and the number of used pages accordingly.
            usedMemory -= PAGES(8 - baseOffset);
            bitmapEntry++;
            pageCount -= 8 - baseOffset;
        }

        // Now keep looping until bitmapEntry exceeds size (select next page and rinse/repeat)
        // or we've deallocated the total number of pages.
        while (pageCount > 0) {
            if (bitmapEntry >= currentFrame->count) {
                currentFrame = reinterpret_cast<ThornhillPhysicalFrame*>(currentFrame->next);
                bitmapEntry = 0;

                if (currentFrame == null) {
                    Kernel::panic("Tried to deallocate non-existent memory. (2)");
                }
            }

            if (pageCount >= 8) {
                if (currentFrame->bitmap[bitmapEntry] == 0) {
                    Kernel::panic("Attempted to free non-allocated memory.");
                }

                // Clear the current bitmap entry (deallocate all the pages).
                currentFrame->bitmap[bitmapEntry] = 0;

                // Update our 'bitmapEntry' value and the number of used pages accordingly.
                usedMemory -= PAGES(8);
                bitmapEntry++;
                pageCount -= 8;
            } else {
                // Create a bitmask, preserving only the bits leading up to the page count – as
                // before with the offset.
                uint8_t mask = 0;
                for (uint8_t i = 0; i < pageCount; i++) {
                    mask |= 1 << i;
                }

                // Now invert that mask such that only bits except those leading up to the page
                // count are preserved.
                mask = ~mask;

                // Next, apply that mask to the current bitmap entry.
                currentFrame->bitmap[bitmapEntry] &= mask;

                // And lastly, update the usedMemory.
                // We needn't update the other variables as after this we'll break out.
                usedMemory -= PAGES(pageCount);
                break;
            }
        }

//        Kernel::panic("Tried to deallocate, but wasn't yet implemented.");
    }

} // namespace ThornhillMemory
