#pragma once

/**
 * If this flag is set, page may be accessed from user-space.
 * NATURALLY, DO NOT SET THIS FLAG FOR KERNEL MEMORY. (MASSIVE SECURITY FLAW).
 */
#define VMM_USERSPACE_PAGE          0b00000001

/**
 * If set, the memory cannot be executed. Useful for allocating pages for kernel or
 * userspace data, e.g., non-executable files or images.
 */
#define VMM_PREVENT_EXECUTE         0b00000010

/**
 * Disables all caching for the page if this flag is set (reading and writing).
 */
#define VMM_DISABLE_ALL_CACHING     0b00000100

/**
 * Disables only write caching for the page. Data is written directly
 * to memory, if this flag is set.
 */
#define VMM_DISABLE_WRITE_CACHING   0b00001000

namespace ThornhillMemory {

    class Virtual {
      private:
        static bool initialized;

      public:
        /**
         * Returns whether or not the virtual memory manager is initialized.
         * @return true, if it is. Otherwise, false.
         */
        static inline bool isInitialized() { return initialized; }

        /**
         * Resets the Virtual Memory Manager so that it can be initialized again.
         */
        static void reset();

        /**
         * Initializes the Virtual Memory Manager (VMM) by setting up the OS-level page tables.
         */
        static void initialize();

        static void allocate(uint64_t bytes, uint8_t flags, uint64_t virtualAddress = null);
    };

}
