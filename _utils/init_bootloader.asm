; e9 fd ff 00 00 00 00 00 00 00 00 00 00 00 00 00
; 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
; [ 29 more lines with sixteen zero-bytes each ]
; 00 00 00 00 00 00 00 00 00 00 00 00 00 00 55 aa

[org 0x7c00]

;
; -- System code
;
MOV ah, 0x0e ; enter tty mode (set register a - higher to: 0x0e).

MOV bp, 0x8000  ; Move away from the bootsector so we don't accidentally overwrite ourselves,
                ; and set bp for the bottom of the stack.
MOV sp, bp      ; Ensure the top of the stack (sp) is aligned to the bottom of the stack (bp).

; Push "Howdy!" to the stack in reverse order.
PUSH "!"
PUSH "y"
PUSH "d"
PUSH "w"
PUSH "o"
PUSH "H" ; Now bp (the bottom of the stack) has been pushed down 6 places.

; While the stack is not empty, pop a character from the stack
; and move it to al for display.
loop1   NOP
        POP bx
        MOV al, bl
        int 0x10
        CMP sp, bp ; If the stack is empty, sp points to bp.
        JNE loop1

;
; -- Infinite loop to cause system hang.
;
jmp $

;
; -- Make code bootable.
;

; Fill with 510 zeroes minus the size of the previous code.
times 510-($-$$) db 0

; Magic number to identify presence of code to BIOS.
dw 0xaa55