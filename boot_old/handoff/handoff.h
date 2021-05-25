#include <stddef.h>
#include <stdint.h>

#include "handoff_memory.h"
#include "../../drivers/graphics.h"

#ifndef TH_BOOT_HANDOFF
#define TH_BOOT_HANDOFF

typedef struct {

    HandoffMemoryMap memoryMap;
    Screen screen;

} ThornhillHandoff;

#endif