#pragma once

extern "C" {
#include "boot/handoff/memory/memory_shared.h"
}

namespace Thornhill {

class ThornhillMemoryManager {

  public:
    static void initialize(HandoffMemoryMap handoffMemoryMap);
};

} // namespace Thornhill
