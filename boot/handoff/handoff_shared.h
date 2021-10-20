#include "../../drivers/graphics.h"
#include "./memory/memory_shared.h"

#pragma once

typedef struct {

    HandoffMemoryMap memoryMap;
    Screen screen;

} ThornhillHandoff;