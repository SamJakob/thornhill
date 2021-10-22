#include "physical.hpp"
#include "lib/thornhill.hpp"
#include "kernel/arch/x86_64/include.hpp"

using namespace Thornhill;

namespace ThornhillMemory {

    bool Physical::initialized = false;
    size_t Physical::usedMemory = 0;
    size_t Physical::totalMemory = 0;

    void Physical::reset() { Physical::initialized = false; }

    void Physical::initialize(HandoffMemoryMap bootMap) {
        Kernel::debug("PMM", "Initializing physical memory manager...");

        if (Physical::initialized) {
            Kernel::panic("Attempted to re-initialize physical memory allocator.");
            return;
        }

        /* Take inventory of the physical memory,
         * start writing kernel memory structures. */
        Kernel::debug("PMM", "Taking inventory of memory structures...");

        // Locate first available page.
        for (uintptr_t segmentIndex = 0; segmentIndex < bootMap.mapSize / sizeof(HandoffMemorySegment); segmentIndex++) {
            if (bootMap.segments[segmentIndex].memoryType == THAvailableMemory &&
                bootMap.segments[segmentIndex].pageCount >= 1 &&
                bootMap.segments[segmentIndex].physicalBaseAddress >= 1) {



//                char buf[512] = {};
//                Kernel::debug("PMM", Thornhill::uitoa(buf, bootMap.segments[segmentIndex].pageCount, 16, 512));
//                memzero((size_t*) &buf[0], 512);
//                Kernel::debug("PMM", Thornhill::uitoa(buf, bootMap.segments[segmentIndex].physicalBaseAddress, 16, 512));

            }
        }

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