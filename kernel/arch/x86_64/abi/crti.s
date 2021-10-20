/* x86_64 SysV (GNU) ABI crti.s */
.section .init
.global _init
.type _init, @function
_init:
    push %rbp
    movq %rsp, %rbp

    # GCC should then insert crtbegin.o's .init section here.

.section .fini
.global _fini
.type _fini, @function
_fini:
    push %rbp
    movq %rsp, %rbp

    # GCC should then insert crtend.o's .fini section here.
