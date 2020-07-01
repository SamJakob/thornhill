; Load 'dh' sectors from drive 'dl' into ES:BX.
[bits 16]
loadFromDisk:
    PUSHA
    PUSH dx

    MOV ah, 0x02    ; ah = int 0x13 function (setting to 0x02 = 'read').
    MOV al, dh      ; al = number of sectors to read.
    MOV cl, 0x02    ; cl <- initial sector. (0x01 = boot, 0x02 = first 'available')
    MOV ch, 0x00    ; ch <- cylinder
                    ; dl <- drive number. Set directly in function call.
                    ; 0x00 = floopy
                    ; 0x01 = floppy2
                    ; 0x80 = hdd
                    ; 0x81 = hdd2
    MOV dh, 0x00    ; dh <- head number

    int 0x13        ; BIOS interrupt.
    JC showDiskError

    POP dx
    CMP al, dh
    JNE showSectorsError
    POPA
    RET

showDiskError:
    MOV bx, DISK_ERROR
    CALL print
    call printNewLine

    ; ah = error code, dl = disk drive that caused the error
    MOV dh, ah
    CALL hexDump

    JMP $

showSectorsError:
    MOV bx, SECTORS_ERROR
    CALL print

DISK_ERROR: db "An error occurred whilst reading the disk.", 0
SECTORS_ERROR: db "There is an unexpected disparity between the number of sectors requested and those read.", 0