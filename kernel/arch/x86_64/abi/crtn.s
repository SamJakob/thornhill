/* x86_64 SysV (GNU) ABI crtn.s */
.section .init
    # GCC should then insert crtbegin.o's .init section here.
    popq %rbp
    ret

.section .fini
    # GCC should then insert crtend.o's .fini section here.
    popq %rbp
    ret
