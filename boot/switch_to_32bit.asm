[bits 16]
switchTo32Bit:
    cli                                ; Disable interrupts.
    lgdt [th_gdt_descriptor]           ; Load the Thornhill GDT for 32-bit protected mode.
    MOV eax, cr0
    OR eax, 0b1                        ; Set PE (Protection Enable) bit in Control Register 0.
    MOV cr0, eax
    JMP TH_GDT_CODE_SEG:init32Bit     ; Jump to 32-bit initialization function.

[bits 32]
init32Bit:
    MOV ax, TH_GDT_DATA_SEG
    MOV ds, ax
    MOV ss, ax
    MOV es, ax
    MOV fs, ax
    MOV gs, ax

    ; Move the stack right to the top of free space.
    MOV ebp, 0x90000
    MOV esp, ebp

    CALL TH_GDT_CODE_SEG:INIT_THORNHILL