[bits 32]
[extern _start]   ; Define calling point in kernel.c
CALL _start       ; Execute the calling point.
JMP $             ; (unreachable)