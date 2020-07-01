; print is called with [bx] set to the base address
; for the string to print.
print:
    PUSHA

    startPrinting:
        MOV al, [bx]
        CMP al, 0       ; Strings are null terminated, so until we reach a null byte,
                        ; we can just keep spewing the string out.
        JE endPrinting
        
        int 0x10        ; If the character was not a null byte, at this point we want to
                        ; print the character.
        ADD bx, 1       ; Now we increment the base address and continue looping.
        JMP startPrinting
    
    endPrinting:
        POPA
        RET

printNewLine:
    PUSHA

    MOV bx, NEWLINE_STRING
    CALL print

    POPA
    RET

NEWLINE_STRING:
    db 0xa, 0xd, 0