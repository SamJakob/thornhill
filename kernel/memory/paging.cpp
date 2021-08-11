#include "paging.hpp"

namespace ThornhillKernel {

    void TLB::flush() {
        __asm__ __volatile__("movq %cr3, %rax \n\t"
                             "movq %rax, %cr3 \n\t");
    }

    void TLB::invalidatePage(uintptr_t address) {
        __asm__ __volatile__("invlpg (%0)" ::"r"(address) : "memory");
    }
    
}