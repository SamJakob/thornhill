#include "paging.h"

void THBSetupPaging(THBKernelSymbols* KernelSymbols) {

    /* The CR3 register holds the memory address of the page tables. */
    uint64_t cr3;
    __asm__ __volatile__ (
        "movq %%cr3, %%rax \n\t"
        "movq %%rax, %0    \n\t"
        : "=m" (cr3)
        : /* no input */
        : "%rax"
    );

    Print(L"%x\n", cr3);
    for (;;){};

}