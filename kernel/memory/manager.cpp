#include "lib/thornhill.cpp"

#include "manager.hpp"
#include "physical.hpp"

using namespace ThornhillMemory;

namespace Thornhill {

void ThornhillMemoryManager::initialize(HandoffMemoryMap handoffMemoryMap) {

    // Prepare the physical memory allocator.
    Physical::initialize(handoffMemoryMap);
    
}

} // namespace Thornhill