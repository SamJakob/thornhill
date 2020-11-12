#include "physical.hpp"
#include "lib/thornhill.cpp"

using namespace Thornhill;

namespace ThornhillMemory {

    uint8_t Physical::MEMORY[1024 * 1024 / 8] = {};

    void Physical::allocate(size_t memorySize) {
        
        if (!IS_PAGE_ALIGNED(memorySize)) return Kernel::panic("Assertion failed: memorySize passed to allocate is not page aligned.", 0);

        

    }

}