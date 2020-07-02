;
; -- Bootstrap
;
[bits 16]                   ; Set 16-bit mode for NASM.
[org 0x7c00]                ; Set origin (bootable code)
    CLI
    MOV ax, 0x0000
    MOV ds, ax
    MOV es, ax
    MOV fs, ax
    MOV gs, ax

    IN al, 0x92
    OR al, 2
    OUT 0x92, al

    MOV ss, ax
    MOV sp, 0xFFFF
    STI

    MOV ax, 0x0003
    int 10h ; Clear screen

    MOV ah, 0x0e ; enter tty mode (set register a - higher to: 0x0e).

    ;MOV bx, pleaseWaitMessage
    ;CALL print
    ;CALL printNewLine

    KERNEL_OFFSET equ 0x1000
    MOV [BOOT_DRIVE], dl ; The BIOS sets dl to the BOOT_DRIVE for us.

    ; Prepare the stack.
    MOV bp, 0x9000
    MOV sp, bp

    ; Load Thornhill kernel.
    CALL loadKernel

    MOV ax, 0x0003
    int 10h ; Clear screen

    ; Switch to protected mode.
    CALL switchTo32Bit

    ; (unreachable)
    JMP $


;
; -- Imports
;
%include "boot/bios/print.asm"
%include "boot/bios/hexdump.asm"
%include "boot/bios/disk.asm"

%include "boot/switch_to_32bit.asm"

%include "boot/32bit/gdt.asm"
%include "boot/32bit/print32.asm"

;
; -- Thornhill System Initialization.
;
[bits 16]
loadKernel:
    MOV bx, loadKernelMessage
    CALL print
    CALL printNewLine

    MOV bx, KERNEL_OFFSET
    MOV dh, 2
    MOV dl, [BOOT_DRIVE]
    CALL loadFromDisk

    RET

[bits 32]
INIT_THORNHILL:
    MOV ebx, protectedModeSwitchMessage ; Indicate switching to protected mode.
    CALL print32                        ; ...this will likely just be an initialization message.

    CALL KERNEL_OFFSET                  ; Now pass control to the kernel.
    JMP $                               ; (unreachable)

;
; -- BIOS Data
;
BOOT_DRIVE db 0 ; BOOT_DRIVE is stored before initializing anything so as to prevent losing its value.
; switch32BitMessage: db "Switching to 32-bit protected mode...", 0
;pleaseWaitMessage: db "Please wait...", 0
loadKernelMessage: db "Loading kernel...", 0
protectedModeSwitchMessage: db "Starting kernel...", 0

;
; -- Boot sector.
;

; Fill with 510 zeroes minus the size of the previous code.
times 510-($-$$) db 0

; Magic number to identify presence of code to BIOS.
dw 0xaa55