.extern interrupt_handler

# Implement a common handler for each of the ISR gates
# allowing for reuse of the call back to kernel code.
interrupt_common:
    # Stage 1: Save CPU state.
    push %rdx
    push %rcx
    push %rbx
    push %rax

    push %rdi
    push %rsi
    push %rbp

    mov %cr4, %rax
    push %rax
    mov %cr3, %rax
    push %rax
    mov %cr2, %rax
    push %rax
    mov %cr0, %rax
    push %rax

    # Stage 2: Call kernel code.
    call interrupt_handler

    # Stage 3: Restore CPU state.
    pop %rax
    mov %rax, %cr0
    pop %rax
    mov %rax, %cr2
    pop %rax
    mov %rax, %cr3
    pop %rax
    mov %rax, %cr4

    pop %rbp
    pop %rsi
    pop %rdi

    pop %rax
    pop %rbx
    pop %rcx
    pop %rdx

    add $16, %rsp
    sti
    iretq

# There is no information on which interrupt was called when
# the handler is run, so a handler is defined for each interrupt.
# 
# Furthermore, as only some interrupts push a handler onto the
# stack, a dummy code is pushed for the interrupts which don't.

.global isr0
.global isr1
.global isr2
.global isr3
.global isr4
.global isr5
.global isr6
.global isr7
.global isr8
.global isr9
.global isr10
.global isr11
.global isr12
.global isr13
.global isr14
.global isr15
.global isr16
.global isr17
.global isr18
.global isr19
.global isr20
.global isr21
.global isr22
.global isr23
.global isr24
.global isr25
.global isr26
.global isr27
.global isr28
.global isr29
.global isr30
.global isr31

# 0: Divide By Zero Exception
isr0:
    cli
    push $0
    push $0
    jmp interrupt_common

# 1: Debug Exception
isr1:
    cli
    push $0
    push $1
    jmp interrupt_common

# 2: Non Maskable Interrupt Exception
isr2:
    cli
    push $0
    push $2
    jmp interrupt_common

# 3: Int 3 Exception
isr3:
    cli
    push $0
    push $3
    jmp interrupt_common

# 4: INTO Exception
isr4:
    cli
    push $0
    push $4
    jmp interrupt_common

# 5: Out of Bounds Exception
isr5:
    cli
    push $0
    push $5
    jmp interrupt_common

# 6: Invalid Opcode Exception
isr6:
    cli
    push $0
    push $6
    jmp interrupt_common

# 7: Coprocessor Not Available Exception
isr7:
    cli
    push $0
    push $7
    jmp interrupt_common

# 8: Double Fault Exception (With Error Code!)
isr8:
    cli
    push $8
    jmp interrupt_common

# 9: Coprocessor Segment Overrun Exception
isr9:
    cli
    push $0
    push $9
    jmp interrupt_common

# 10: Bad TSS Exception (With Error Code!)
isr10:
    cli
    push $10
    jmp interrupt_common

# 11: Segment Not Present Exception (With Error Code!)
isr11:
    cli
    push $11
    jmp interrupt_common

# 12: Stack Fault Exception (With Error Code!)
isr12:
    cli
    push $12
    jmp interrupt_common

# 13: General Protection Fault Exception (With Error Code!)
isr13:
    cli
    push $13
    jmp interrupt_common

# 14: Page Fault Exception (With Error Code!)
isr14:
    cli
    push $14
    jmp interrupt_common

# 15: Reserved Exception
isr15:
    cli
    push $0
    push $15
    jmp interrupt_common

# 16: Floating Point Exception
isr16:
    cli
    push $0
    push $16
    jmp interrupt_common

# 17: Alignment Check Exception
isr17:
    cli
    push $0
    push $17
    jmp interrupt_common

# 18: Machine Check Exception
isr18:
    cli
    push $0
    push $18
    jmp interrupt_common

# 19: Reserved
isr19:
    cli
    push $0
    push $19
    jmp interrupt_common

# 20: Reserved
isr20:
    cli
    push $0
    push $20
    jmp interrupt_common

# 21: Reserved
isr21:
    cli
    push $0
    push $21
    jmp interrupt_common

# 22: Reserved
isr22:
    cli
    push $0
    push $22
    jmp interrupt_common

# 23: Reserved
isr23:
    cli
    push $0
    push $23
    jmp interrupt_common

# 24: Reserved
isr24:
    cli
    push $0
    push $24
    jmp interrupt_common

# 25: Reserved
isr25:
    cli
    push $0
    push $25
    jmp interrupt_common

# 26: Reserved
isr26:
    cli
    push $0
    push $26
    jmp interrupt_common

# 27: Reserved
isr27:
    cli
    push $0
    push $27
    jmp interrupt_common

# 28: Reserved
isr28:
    cli
    push $0
    push $28
    jmp interrupt_common

# 29: Reserved
isr29:
    cli
    push $0
    push $29
    jmp interrupt_common

# 30: Reserved
isr30:
    cli
    push $0
    push $30
    jmp interrupt_common

# 31: Reserved
isr31:
    cli
    push $0
    push $31
    jmp interrupt_common
