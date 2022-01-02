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
        assert(minBaseAddress > 0);
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

            Kernel::debugf("PMM", "last page: %x", lastPage);

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

            assert(Physical::physicalInventoryBase != null);

            ThornhillPhysicalFrame* previousFrame = nullptr;
            auto* currentFrame = (ThornhillPhysicalFrame*)inventoryPage;

            uint64_t currentPage = _firstAvailablePage(bootMap, inventoryPage + PAGES(1));

            while (currentPage <= lastPage) {
                if (currentPage == null) {
                    Kernel::debug("PMM", "    No more usable pages.");
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
                // inventoryOffset += 24; // add the frame header to the offset.

                // Calculate the size of the bitmap required to hold this contiguous section of pages. If we're at this point of execution, we can guarantee that the current inventory offset will have the appropriate frame headers set.
                uint16_t bitmapSize = max(
                    (uint64_t) min(
                        (uint64_t) ceilToN((long double)pageCount / 8, 8),
                        (inventoryPage + PAGES(1)) - ((uint64_t)(&currentFrame) + 24)
                    ),
                    0UL /* ensure value stored in uint is, in fact, >= 0 */
                );
                auto* bitmap = (uint8_t*)(&currentFrame + 24);
                // Zero the bitmap representing this section.
                memzero(&currentFrame + 24, bitmapSize);

                // Determine whether the current bitmap frame filled the inventory page.
                bool didFillInventoryPage = (uint64_t) &currentFrame + 24 + bitmapSize == (uint64_t) inventoryPage + PAGES(1);
                if (didFillInventoryPage) Kernel::debug("PMM", "filled inventory page");

                // Update the current frame to reflect the newly defined bitmap.
                currentFrame->count = bitmapSize / 8;
                currentFrame->bitmap = bitmap;

                Kernel::debugf("PMM", "    Adding %d page(s) (base = %x).", currentFrame->count,
                               currentFrame->base);
                currentPage += PAGES(currentFrame->count);
                Kernel::debugf("PMM", "    Jumping to page %x", currentPage);

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

                    // inventoryOffset = 0;
                    currentFrame = reinterpret_cast<ThornhillPhysicalFrame*>(inventoryPage);
                    Kernel::debugf("PMM", "    Kernel page filled, seeking next inventory page: %x",
                                   inventoryPage);

                    // Then, start processing the next available page.
                    currentPage = _firstAvailablePage(bootMap, inventoryPage + PAGES(1));
                } else {
                    // Start processing the next available page.
                    currentFrame = currentFrame + 24 + (bitmapSize / 8);
                    currentPage = _firstAvailablePage(bootMap, currentPage + PAGES(1));
                    if (currentPage != null) Kernel::debugf("PMM",
                                   "    Kernel page has remainder. Starting again from next page: %x",
                                   currentPage);
                }
            }
        }

        // Check inventory.
        {
            Kernel::debug("PMM", "Checking inventory...");
            auto* currentFrame = reinterpret_cast<ThornhillPhysicalFrame*>(Physical::physicalInventoryBase);
            Kernel::debug("PMM", "    Loading page frame at base address.");

            while (currentFrame != null) {
                Physical::totalMemory += PAGES(currentFrame->count);
                Kernel::debugf("PMM", "      Cluster base: %x", currentFrame->base);
                Kernel::debugf("PMM", "      Inventory page: %x", PAGE_ALIGN_DOWN((uint64_t) &currentFrame));
                Kernel::debugf("PMM", "      Has %d pages.", currentFrame->count);
                Kernel::debugf("PMM", "      Has %d KiB memory.", currentFrame->count * 4);
                currentFrame = reinterpret_cast<ThornhillPhysicalFrame*>(currentFrame->next);
                if (currentFrame != null) Kernel::debug("PMM", "    Jumping to next page frame.");
            }
        }

        Kernel::debug("PMM", "PMM is ready.");
        Kernel::debugf("PMM", "\tPhysical memory: %d KiB (bytes)", Physical::totalMemory / 1024, Physical::totalMemory);

    }

    void Physical::allocate(size_t memorySize) {

        if (!IS_PAGE_ALIGNED(memorySize))
            return Kernel::panic("Assertion failed: memorySize passed to allocate is not page aligned.",
                                 0);

    }

} // namespace ThornhillMemory
