.global load_gdt
load_gdt:
    lgdt (%rdi)         # load GDT, rdi (1st argument) contains the gdt_ptr
    mov $0x40, %ax      # TSS segment is 0x40
    ltr %ax             # load TSS
    mov $0x08, %ax      # kernel data segment is 0x08
    mov %ax,   %ds      # load kernel data segment in data segment registers
    mov %ax,   %es
    mov %ax,   %fs
    mov %ax,   %gs
    mov %ax,   %ss
    # popq %rdi           # pop the return address
    # mov $0x10, %rax     # kernel code segment is 0x10
    # pushq %rax          # push the kernel code segment
    # pushq %rdi          # push the return address again
    retq                # do a far return, like a normal return but
                        # - pop an extra argument of the stack
                        # - and load it into CS
