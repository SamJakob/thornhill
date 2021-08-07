#include "../../drivers/graphics.h"
#include "./memory/memory_shared.h"

#ifndef TH_BOOT_HANDOFF
#define TH_BOOT_HANDOFF

typedef struct {

    HandoffMemoryMap memoryMap;
    Screen screen;

} ThornhillHandoff;

#endif