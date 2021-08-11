.global load_gdt
load_gdt:

    lgdtq (%rdi)         # load GDT from the pointer in %rdi (1st argument)

    mov $0x40, %ax
    ltr %ax             # load TSS

    mov $0x10, %ax      # kernel data segment is 0x10
    mov %ax,   %ds      # load kernel data segment in data segment registers
    mov %ax,   %es
    mov %ax,   %fs
    mov %ax,   %gs
    mov %ax,   %ss
    
    # pop the return address into rdi so it can be pushed after far jump params
    popq %rdi

    # push new current segment value (cs)
    pushq $0x08

    # push return address
    pushq %rdi

    lretq
