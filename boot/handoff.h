#include <stddef.h>
#include <stdint.h>

#include "../drivers/graphics.h"

#ifndef TH_BOOT_HANDOFF
#define TH_BOOT_HANDOFF

typedef struct {
    
    void* memory_map;
    size_t map_size;
    size_t desc_size;

} HandoffEfiMemoryMap;

typedef struct {

    HandoffEfiMemoryMap* memoryMap;
    Screen screen;

} ThornhillHandoff;

#endif