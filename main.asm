; e9 fd ff 00 00 00 00 00 00 00 00 00 00 00 00 00
; 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
; [ 29 more lines with sixteen zero-bytes each ]
; 00 00 00 00 00 00 00 00 00 00 00 00 00 00 55 aa

;
; -- System code
;
MOV ax, 0x0003
int 10h ; Clear screen

MOV ah, 0x0e ; enter tty mode (set register a - higher to: 0x0e).

[org 0x7c00]
    KERNEL_OFFSET equ 0x1000
    MOV [BOOT_DRIVE], dl ; The BIOS sets dl to the BOOT_DRIVE for us.

    ; Prepare the stack.
    MOV bp, 0x9000
    MOV sp, bp

    ; Load Thornhill kernel.
    CALL loadKernel

    ; Switch to protected mode.
    CALL switchTo32Bit

    ; (unreachable)
    jmp $


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
loadKernelMessage: db "Loading kernel...", 0
protectedModeSwitchMessage: db "Initializing Thornhill...", 0

;
; -- Boot sector.
;

; Fill with 510 zeroes minus the size of the previous code.
times 510-($-$$) db 0

; Magic number to identify presence of code to BIOS.
dw 0xaa55