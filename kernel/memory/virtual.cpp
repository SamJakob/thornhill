#include <thornhill>

#include "virtual.hpp"

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

        Virtual::initialized = true;
    }

}
