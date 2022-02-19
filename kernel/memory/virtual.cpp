#include <thornhill>

#include "virtual.hpp"
#include "physical.hpp"

using namespace Thornhill;

namespace ThornhillMemory {

    bool Virtual::initialized = false;

    void Virtual::reset() {
        Virtual::initialized = false;
    }

    void Virtual::initialize() {
        Kernel::debug("VMM", "Initializing virtual memory manager...");

        if (Virtual::initialized) {
            Kernel::panic("Attempted to re-initialize VMM in initialized state.");
        }

        // Set up page tables.

        Virtual::initialized = true;
    }

}
