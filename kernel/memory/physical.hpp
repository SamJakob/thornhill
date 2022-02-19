#include <thornhill>
#include "boot/handoff/memory/memory_shared.h"

#pragma once

#define PHYSICAL_FRAME_HEADER_SIZE 32

namespace ThornhillMemory {

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
         * A bitmap of metadata about this physical frame.
         */
        uint64_t metadata;

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

        /**
         * Returns whether or not the physical memory manager is initialized.
         * @return true, if it is. Otherwise false.
         */
        static inline bool isInitialized() { return initialized; }

        /**
         * Returns the number of bytes of physical memory currently in use by the PMM.
         * @return The number of bytes in use by the PMM in a size_t.
         */
        static inline size_t getUsedMemory() { return usedMemory; }

        /**
         * Returns the total amount of physical memory available to the PMM. This is calculated
         * on initialization of the PMM based on the number of available pages.
         * @return The number of bytes totally available to the PMM in a size_t.
         */
        static inline size_t getTotalMemory() { return totalMemory; }

        //
        // Memory Management Functions
        //

        /**
         * Resets the memory manager so that it can be initialized again.
         */
        static void reset();

        /**
         * Initializes the Physical Memory Manager (PMM) with the specified memory map from the
         * firmware.
         * @param bootMap The bootmap passed to the OS by the bootloader.
         */
        static void initialize(HandoffMemoryMap bootMap);

        /**
         * Attempts to allocate the specified pageCount as a contiguous chunk of memory.
         * If it fails, nullptr is returned, otherwise a pointer to the base address of the
         * specified pageCount is returned.
         *
         * @param pageCount The amount of pages to allocate.
         * @return A pointer; either to null or the base of the allocated memory.
         */
        static void* allocate(size_t pageCount);

        static void deallocate(void* base, size_t pageCount);
    };

} // namespace ThornhillMemory
