; hexDump is called with [dx] set to the base address
; for the string to print.
hexDump:
    PUSHA
    MOV cx, 0            ; Initialize counter variable.

    ; Get the last character of 'dx', convert to ASCII.
    ; ASCII numeric characters (0-9) start at 0x30 so we can just add 0x30.
    ; Alphabetic characters (A-F) start at 0x41, so we can just add 0x40. 
    hexLoop:
        CMP cx, 4
        JE endHexDump

        ; Convert the last character of 'dx' to ASCII
        MOV al, dl       ; Move dx to our working register.
        AND al, 0x000F   ; Get last character by masking first three to zero.
        ADD al, 0x30     ; Add 0x30 to convert to an ASCII digit.
        CMP al, 0x39     ; If it's greater than ASCII '9', we know it's a letter value (10-15)
        JLE hexLoopStep2 ; If not a letter value, jump straight to prepararation for output,
        ADD al, 7        ; Otherwise, add 0x11 to transform it into one of the ASCII letters, A-F.

        ; Our character is now in al.

    ; We now write our character into the text at position
    ; (text pointer - counter)
    hexLoopStep2:
        MOV bx, HEX_OUT + 5  ; Move our text pointer to the end of HEX_OUT.
        SUB bx, cx           ; Subtract the counter value from the text pointer.
        MOV [bx], al         ; Move our character into place.
        ROR dx, 4            ; Shift the last character to the beginning. (e.g. 0x1234 -> 0x4123)
                             ; - ready for hexLoop to process the next character.
        ADD cx, 1
        JMP hexLoop

    endHexDump:
        MOV bx, HEX_OUT
        CALL print

        POPA
        RET

    HEX_OUT:
        db '0x0000', 0  ; Reserved memory for the hex output.