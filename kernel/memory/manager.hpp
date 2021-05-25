#include <stddef.h>
#include <stdint.h>

#pragma once

extern "C" {
#include "boot/handoff/memory.h"
}

namespace Thornhill {

class ThornhillMemoryManager {

  public:
    static void initialize(HandoffMemoryMap handoffMemoryMap);
};

} // namespace Thornhill
