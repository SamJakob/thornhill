#include "physical.hpp"
#include "lib/thornhill.hpp"
#include "kernel/arch/x86_64/include.hpp"

#define THMEM_AVAILABLE 0xFF
#define THMEM_UNAVAILABLE 0x00

namespace ThornhillMemory {

uint8_t Physical::MEMORY[1024 * 1024 / 8] = {};
bool Physical::isPhysicalAllocatorInitialized = false;

size_t Physical::usedMemory = 0;
size_t Physical::totalMemory = 0;

void Physical::reset() { Physical::isPhysicalAllocatorInitialized = false; }

void Physical::initialize(HandoffMemoryMap handoffMemoryMap) {
    if (Physical::isPhysicalAllocatorInitialized) {
        Thornhill::Kernel::panic("Attempted to re-initialize physical memory allocator.");
        return;
    }

    // Set each bit of memory to unavailable.
    for (size_t i = 0; i < sizeof(MEMORY); i++) {
        MEMORY[i] = THMEM_UNAVAILABLE;
    }

    // Now loop through the memory map and set as available accordingly.


    Physical::usedMemory = usedMemory;
    Physical::totalMemory = totalMemory;

    Physical::isPhysicalAllocatorInitialized = true;
}

void Physical::allocate(size_t memorySize) {

    if (!IS_PAGE_ALIGNED(memorySize))
        return Thornhill::Kernel::panic("Assertion failed: memorySize passed to allocate is not page aligned.",
                             0);



}

} // namespace ThornhillMemory