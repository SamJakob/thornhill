#include <stdint.h>

#ifndef TH_KERNEL_MEMORY_PAGING
#define TH_KERNEL_MEMORY_PAGING

namespace ThornhillKernel {

    struct __attribute__((packed)) ThornhillPageTableEntry {
        
        /**
         * @brief Whether the page is currently in memory.
         */
        int present : 1;

        /**
         * @brief Whether the system may write to this page.
         */
        int writable : 1;

        /**
         * @brief Whether user-space code may access this page (= true/1) or only
         * kernel-space code may access the page (= false/0).
         */
        int userAccessible : 1;

        /**
         * @brief Whether writes to this page bypass the cache and are written
         * directly to memory (= true/1) or not (= false/0).
         */
        int writeThroughCaching : 1;

        /**
         * @brief Whether caching is entirely disabled for this page.
         */
        int disableCache : 1;

        /**
         * @brief Set by the CPU when the page is accessed.
         */
        int accessed : 1;

        /**
         * @brief Set by the CPU when a write to the page occurs.
         */
        int dirty : 1;

        /**
         * @brief Must be a 0 in P1 and P4, creates a 1GiB page in P3 and creates
         * a 2MiB page in P2. (Where Px is the page table level x.)
         */
        int hugePage : 1;

        /**
         * @brief If set, the page isn't flushed from caches on address space
         * switch, provided the PGE bit of the CR4 register is set.
         */
        int global : 1;

        /**
         * @brief Currently unused but reserved for use by the OS.
         * 
         * @see ThornhillPageTableEntry::osFlags2
         */
        int osFlags1 : 3;

        /**
         * @brief Stores the 52-bit physical address of the next page table or final
         * page frame.
         * 
         * @note We only use 39 bits to store this address because the first 11
         * bits can be omitted as we always point to a 4096-byte-(page)-aligned and
         * likewise, the high-end bits can be omitted as x86-64 only actually
         * supports 52-bit physical addresses.
         */
        uint64_t physicalAddress : 39;

        /**
         * @brief Currently unused but reserved for use by the OS.
         * 
         * @see ThornhillPageTableEntry::osFlags1
         */
        int osFlags2 : 10;

        /**
         * @brief Whether executing code on this page should be forbidden (= true/1)
         * or not (= false/0).
         * 
         * The NXE bit in the EFER register must be set.
         */
        int noExecute : 1;

    };

    struct __attribute__((aligned(4096))) ThornhillPageTable {
        struct ThornhillPageTableEntry entries[512];
    };

    /**
     * @brief Transatlantic Lobster Buffet.
     * (also known to the uncultured as the Translation Lookaside Buffer.)
     */
    class TLB {
        public:
            /**
             * @brief Flushes the system TLB completely by reloading the CR3
             * register.
             */
            static void flush();

            /**
             * @brief Invalidates a specific page in the TLB so that it's
             * loaded from the page table on the next access.
             * 
             * @param address The address of the page that should be flushed
             * from the TLB.
             */
            static void invalidatePage(uintptr_t address);
    };

}

#endif