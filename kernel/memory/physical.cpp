#include <algorithm>
#include <math>
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
    static uint64_t _firstAvailablePage(HandoffMemoryMap& bootMap, uint64_t minBaseAddress = 1) {
        assert(minBaseAddress > 0)
        uint64_t baseAddress = null;

        for (uintptr_t segmentIndex = 0; segmentIndex < bootMap.mapSize / sizeof(HandoffMemorySegment); segmentIndex++) {
            if (bootMap.segments[segmentIndex].memoryType == THAvailableMemory &&
                bootMap.segments[segmentIndex].pageCount >= 1 &&
                bootMap.segments[segmentIndex].physicalBaseAddress >= minBaseAddress) {

                // Grab the smallest non-zero base address.
                if (baseAddress == null || baseAddress > bootMap.segments[segmentIndex].physicalBaseAddress) {
                    // firstPageSegmentIndex = segmentIndex;
                    baseAddress = bootMap.segments[segmentIndex].physicalBaseAddress;
                }

            }
        }

        return baseAddress;
    }

    static uint64_t _lastAvailablePage(HandoffMemoryMap& bootMap) {
        uint64_t baseAddress = null;

        for (uintptr_t segmentIndex = 0; segmentIndex < bootMap.mapSize / sizeof(HandoffMemorySegment); segmentIndex++) {
            if (bootMap.segments[segmentIndex].memoryType == THAvailableMemory &&
                bootMap.segments[segmentIndex].pageCount >= 1 &&
                bootMap.segments[segmentIndex].physicalBaseAddress >= 1) {

                // Grab the largest base address of that segment's pages.
                uint64_t largestBaseAddress = bootMap.segments[segmentIndex].physicalBaseAddress +
                                              PAGES(bootMap.segments[segmentIndex].pageCount - 1);

                if (baseAddress == null || largestBaseAddress > baseAddress) {
                    baseAddress = largestBaseAddress;
                }

            }
        }

        return baseAddress;
    }

    /**
     * Essentially the same as `_bootMapSegmentForPageAddress`, except it returns a true or false
     * value indicating whether or not the address is available instead.
     *
     * @param bootMap The boot-time memory map that is passed into the PMM
     * initialization method.
     * @param address The address to check.
     * @return True, if there is a boot-map segment containing that memory
     * address, otherwise false.
     */
    static uint64_t _bootMapHasSegmentForPageAddress(HandoffMemoryMap& bootMap, uint64_t address) {
        for (uintptr_t segmentIndex = 0; segmentIndex < bootMap.mapSize / sizeof(HandoffMemorySegment); segmentIndex++) {
            if (
                bootMap.segments[segmentIndex].physicalBaseAddress >= address &&
                address < (bootMap.segments[segmentIndex].physicalBaseAddress + PAGES(bootMap.segments[segmentIndex].pageCount))
            ) {
                return true;
            }
        }

        return false;
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
        for (uintptr_t segmentIndex = 0; segmentIndex < bootMap.mapSize / sizeof(HandoffMemorySegment); segmentIndex++) {
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
    size_t Physical::physicalInventoryBase = 0;

    void Physical::reset() { Physical::initialized = false; }

    void Physical::initialize(HandoffMemoryMap bootMap) {
        Kernel::debug("PMM", "Initializing physical memory manager...");

        if (Physical::initialized) {
            Kernel::panic("Attempted to re-initialize PMM.");
        }

        // Take inventory.
        {
            Kernel::debug("PMM", "Taking inventory of memory structures...");

            // Locate the first available page.
            uint64_t firstPage = _firstAvailablePage(bootMap);
            // ...and locate the last available page.
            uint64_t lastPage = _lastAvailablePage(bootMap);

            if (firstPage == null) {
                // If we failed to find a page (indicated by firstPage being null), we should panic
                // because this means there's absolutely no pages available, meaning there's no
                // available system memory at all.
                Kernel::panic("No available memory.");
            }

            // Take the first page for PMM inventory usage and set physicalInventoryBase to its
            // base address.
            Kernel::debugf("PMM", "Taking page (base = %x) for PMM usage.", firstPage);
            uint64_t inventoryPage = firstPage;
            Physical::physicalInventoryBase = firstPage;

            assert(Physical::physicalInventoryBase != null)

                /** The working offset in the inventory page to store the next physical frame. */
                uint8_t inventoryOffset = 0;

            ThornhillPhysicalFrame* previousFrame = nullptr;
            auto* currentFrame = (ThornhillPhysicalFrame*)inventoryPage;

            uint64_t currentPage = _firstAvailablePage(bootMap, inventoryPage + PAGES(1));

            while (currentPage < lastPage) {
                if (currentPage == null) {
                    Kernel::debug("PMM", "No more usable pages.");
                    break;
                }

                /* Take inventory in the PMM inventory page until it's full. */

                // Load the firmware information about the current memory segment.
                uint64_t currentSegmentIndex = _bootMapSegmentForPageAddress(bootMap, currentPage);
                HandoffMemorySegment* segment = &bootMap.segments[currentSegmentIndex];
                // Retrieve the page count from the firmware segment information.
                uint64_t pageCount = segment->pageCount;

                // Initialize the physical frame that will store the bitmap.
                currentFrame->base = currentPage;
                inventoryOffset += 24; // add the frame header to the offset.

                // Calculate the size of the bitmap required to hold this contiguous section of pages. If we're at this point of execution, we can guarantee that the current inventory offset will have the appropriate frame headers set.
                uint16_t bitmapSize = max(min(ceilToN((long double)pageCount / 8, 8),
                                              (long long)TH_ARCH_PAGE_SIZE - inventoryOffset),
                                          0LL /* ensure value stored in uint is, in fact, >= 0 */);
                auto* bitmap = (uint8_t*)(inventoryPage + inventoryOffset);
                // Zero the bitmap representing this section.
                memzero(bitmap, bitmapSize);

                // Determine whether the current bitmap frame filled the inventory page.
                bool didFillInventoryPage = bitmapSize == TH_ARCH_PAGE_SIZE - inventoryOffset;

                // Update the current frame to reflect the newly defined bitmap.
                currentFrame->count = bitmapSize / 8;
                currentFrame->bitmap = bitmap;

                Kernel::debugf("PMM", "Adding %d page(s) (base = %x).", currentFrame->count,
                               currentFrame->base);
                currentPage += PAGES(currentFrame->count);
                Kernel::debugf("PMM", "Jumping to page %x", currentPage);

                // Reset the current frame, update the previous frame if it exists and then replace
                // the 'previousFrame' reference with the current frame.
                if (previousFrame != nullptr)
                    previousFrame->next = reinterpret_cast<uint64_t>(currentFrame);
                previousFrame = currentFrame;

                // IF it filled the inventory page, the new current frame should be one in the next
                // inventory page after the current one (which should be discovered here).
                // OTHERWISE, the new current frame should be the one in the current inventory page.
                if (didFillInventoryPage) {
                    // Take the next available page as an inventory page.
                    inventoryPage = _firstAvailablePage(bootMap, currentPage + PAGES(1));
                    // If an inventory page couldn't be found, break early as we've exhausted available memory.
                    if (inventoryPage == null)
                        break;

                    inventoryOffset = 0;
                    currentFrame = reinterpret_cast<ThornhillPhysicalFrame*>(inventoryPage);
                    Kernel::debugf("PMM", "Kernel page filled, seeking next inventory page: %x",
                                   inventoryPage);

                    // Then, start processing the next available page.
                    currentPage = _firstAvailablePage(bootMap, inventoryPage + PAGES(1));
                } else {
                    // Start processing the next available page.
                    currentFrame = currentFrame + 24 + (bitmapSize / 8);
                    currentPage = _firstAvailablePage(bootMap, currentPage + PAGES(1));
                    Kernel::debugf("PMM",
                                   "Kernel page has remainder. Starting again from next page: %x",
                                   currentPage);
                }
            }
        }

        // Check inventory.
        {
            Kernel::debug("PMM", "Checking inventory...");
            auto* currentFrame = reinterpret_cast<ThornhillPhysicalFrame*>(Physical::physicalInventoryBase);

            while (currentFrame != null) {
                Physical::totalMemory += PAGES(currentFrame->count);
                currentFrame = reinterpret_cast<ThornhillPhysicalFrame*>(currentFrame->next);
            }
        }

        Kernel::debug("PMM", "PMM is ready.");
        Kernel::debugf("PMM", "\tPhysical memory: %d bytes", Physical::totalMemory);

    }

    void Physical::initializeLegacy(HandoffMemoryMap bootMap) {
        Kernel::debug("PMM", "Initializing physical memory manager...");

        if (Physical::initialized) {
            Kernel::panic("Attempted to re-initialize PMM.");
        }

        /* Take inventory of the physical memory,
         * start writing kernel memory structures. */
        Kernel::debug("PMM", "Taking inventory of memory structures...");

        // Locate first available page with a non-zero memory address.
        uint64_t baseAddress = _firstAvailablePage(bootMap, 1);
        // ...and locate the last available page.
        uint64_t lastBaseAddress = _lastAvailablePage(bootMap);

        // If we failed to find a page, we need to panic as there's nowhere for the PMM to use and
        // more importantly absolutely no usable memory in the system.
        if (baseAddress == null) {
            Kernel::panic("No available memory.");
        } else Kernel::debugf("PMM", "Taking first page for PMM usage. %x", baseAddress);

        // The base address of the page we're currently 'up to', with respect to taking
        // inventory. We'll initially start with the page after the one we reserved for
        // the PMM's inventory.
        uint64_t currentBaseAddress = baseAddress;

        // The current page serving as an 'inventory' page. (i.e., a page that the PMM
        // uses to store information about other pages.)
        struct {
            // The base address of the page we're currently using to take inventory.
            uintptr_t base;
            // The current offset into the page.
            uint16_t offset;
        } currentInventory = { .base = null, .offset = 0 };

        ThornhillPhysicalFrame* prevFrame = nullptr;

        while (currentBaseAddress < lastBaseAddress) {

            // If the current base address of the inventory page is null (0), we need
            // to reserve the current page for our use.
            if (currentInventory.base == null) {
                currentInventory.base = currentBaseAddress;
                currentInventory.offset = 0;

                // Then increment the current base address, to start taking inventory
                // of the next page.
                currentBaseAddress += PAGES(1);
            }

            /* Take inventory in our inventory page. */

            // Grab the section for the current page.
            uint64_t currentSegmentIndex = _bootMapSegmentForPageAddress(bootMap, currentBaseAddress);
            HandoffMemorySegment* segment = &bootMap.segments[currentSegmentIndex];

            // The number of pages, to take inventory of, is the number of pages in the segment,
            // minus the number of pages either reserved for the PMM at the start of the segment,
            // or the number of pages that have already been taken inventory of.
//            uint64_t pageCount = segment->pageCount - (
//                (currentBaseAddress - segment->physicalBaseAddress) /
//                TH_ARCH_PAGE_SIZE
//            );
            uint64_t pageCount = segment->pageCount;

            // Calculate the size of the bitmap required to store the current segment.
            uint16_t bitmapSize = min(
                ceilToN((long double) pageCount / 8, 8),
                (long long int) TH_ARCH_PAGE_SIZE - 24
            );
            auto* bitmap = (uint8_t*) (currentBaseAddress + 24);
            memzero(bitmap, bitmapSize);

            auto* inventoryAddress = (ThornhillPhysicalFrame*) (currentInventory.base + currentInventory.offset);
            *(inventoryAddress) = {
                .base = currentBaseAddress,
                .next = null,
                .count = pageCount,
                .bitmap = bitmap
            };
            if (prevFrame != nullptr) prevFrame->next = (uint64_t)inventoryAddress;
            prevFrame = inventoryAddress;

            currentInventory.offset += 24 + bitmapSize;
            currentBaseAddress += PAGES(pageCount);
            Kernel::debugf("PMM", "%x", segment->pageCount);
            Kernel::debugf("PMM", "%x", segment->physicalBaseAddress);
            Kernel::debugf("PMM", "%x", (currentBaseAddress - segment->physicalBaseAddress) /
                                            TH_ARCH_PAGE_SIZE);
            Kernel::debugf("PMM", "%x", pageCount);
            Kernel::debugf("PMM", "%x", PAGES(pageCount));
            Kernel::debugf("PMM", "%x", currentBaseAddress);

            for (;;);

            /* End: Take inventory in our inventory page. */

            // If we exceed the amount of data the current page can hold, we'll set our
            // base to null to 'request' a new page the next time the loop occurs.
            if (currentInventory.offset > TH_ARCH_PAGE_SIZE) {
                currentInventory.base = null;
            }

        }
//        uintptr_t pmmStructureBase = currentPage;
//        currentPage += PAGES(1);
//
//        for (;pmmStructureBase < pmmStructureBase + PAGES(1);) {
//            auto* pmmStructure = (ThornhillPhysicalFrame*) pmmStructureBase;
//
//            uint64_t currentPageSegmentIndex = _bootMapSegmentForPageAddress(bootMap, pmmStructureBase);
//        }

//        Physical::usedMemory = usedMemory;
//        Physical::totalMemory = totalMemory;

        Physical::initialized = true;
    }

    void Physical::allocate(size_t memorySize) {

        if (!IS_PAGE_ALIGNED(memorySize))
            return Kernel::panic("Assertion failed: memorySize passed to allocate is not page aligned.",
                                 0);

    }

} // namespace ThornhillMemory
