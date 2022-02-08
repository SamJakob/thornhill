#include "paging.hpp"

#include "kernel/arch/x86_64/include.hpp"
#include "kernel/arch/memory.h"

#include <kernel>

namespace ThornhillKernel {

    void TLB::flush() {
        __asm__ __volatile__("movq %cr3, %rax \n\t"
                             "movq %rax, %cr3 \n\t");
    }

    void TLB::invalidatePage(uint64_t address) {
        assert(IS_PAGE_ALIGNED(address));
        if (!IS_PAGE_ALIGNED(address)) return;

        __asm__ __volatile__("invlpg (%0)" ::"r"(address) : "memory");
    }
    
}
