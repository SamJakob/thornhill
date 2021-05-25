#include <stddef.h>
#include <stdint.h>

extern "C" {
#include "boot/handoff/memory.h"
}

#pragma once

namespace ThornhillMemory {

/*
struct PhysicalMemorySlab {
    PhysicalMemorySlab* next;
    PhysicalMemorySlab* prev;
    bool allocated;
    size_t size;
};
*/

class Physical {

  private:
    static bool isPhysicalAllocatorInitialized;
    static uint8_t MEMORY[1024 * 1024 / 8];

    static size_t totalMemory;
    static size_t usedMemory;

  public:
    //
    // Getters
    //
    static bool isInitialized() { return isPhysicalAllocatorInitialized; }

    static size_t getUsedMemory() { return usedMemory; }

    static size_t getTotalMemory() { return totalMemory; }

    //
    // Memory Management Functions
    //
    static void reset();

    static void initialize(HandoffMemoryMap handoffMemoryMap);

    static void allocate(size_t memorySize);
};

} // namespace ThornhillMemory