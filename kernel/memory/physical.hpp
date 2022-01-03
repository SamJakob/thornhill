extern "C" {
#include "boot/handoff/memory/memory_shared.h"
}

#pragma once

/**
 * A structure used by the Physical Memory Manager (PMM) to store information about a given
 * cluster of pages.
 */
struct __attribute__((packed)) ThornhillPhysicalFrame {

    /**
     * The physical base address of the first page that this frame refers to.
     */
    uint64_t base;

    /**
     * The virtual base address of the next ThornhillPhysicalFrame structure.
     */
    uint64_t next;

    /**
     * The number of pages in the current frame. This frame, therefore refers to all memory
     * from base to base + (count * TH_ARCH_PAGE_SIZE).
     */
    uint64_t count;

    /**
     * The bitmap of page usage information of the pages represented by this frame. Each entry in
     * the bitmap, bit N, refers to the page starting at base + (N * TH_ARCH_PAGE_SIZE). e.g., bit
     * 0 refers to the base address, bit 1 refers to the page after the base address, etc.
     *
     * A bit value of 1 denotes that the page is in use. A bit value of 0 denotes that the page is
     * not in use.
     *
     * Any entries in the bitmap after N = `count` are undefined.
     */
    uint8_t* bitmap;

};

namespace ThornhillMemory {

    class Physical {

      private:
        static bool initialized;

        /**
         * The base address of the first physical memory page holding a page inventory.
         */
        static size_t inventoryBase;

        /**
         * The total amount of physical memory available to the system.
         */
        static size_t totalMemory;
        /**
         * The total amount of physical memory used by the system.
         */
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
