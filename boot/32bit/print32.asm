[bits 32]

VIDEO_MEMORY equ 0xb8000
WHITE_ON_BLACK equ 0x0f ; Color byte for each character.

print32:
    PUSHA
    MOV edx, VIDEO_MEMORY

    startPrinting32:
        MOV al, [ebx]
        MOV ah, WHITE_ON_BLACK

        CMP al, 0
        JE endPrinting32

        MOV [edx], ax
        ADD ebx, 1 ; Proceed to next character
        ADD edx, 2 ; ...and write to next position in video memory.

        JMP startPrinting32
    
    endPrinting32:
        POPA
        RET