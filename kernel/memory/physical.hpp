extern "C" {
#include "boot/handoff/memory/memory_shared.h"
}

#pragma once

struct __attribute__((packed)) ThornhillPhysicalFrame {
    uint64_t base;
    uint64_t count;
    uint64_t next;
};

namespace ThornhillMemory {

    class Physical {

      private:
        static bool initialized;

        static size_t totalMemory;
        static size_t usedMemory;

      public:
        //
        // Getters
        //
        static bool isInitialized() { return initialized; }

        static size_t getUsedMemory() { return usedMemory; }

        static size_t getTotalMemory() { return totalMemory; }

        //
        // Memory Management Functions
        //
        static void reset();

        static void initialize(HandoffMemoryMap bootMap);

        static void allocate(size_t memorySize);
    };

} // namespace ThornhillMemory