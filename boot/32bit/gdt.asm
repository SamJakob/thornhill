th_gdt_start:
    DD 0x0
    DD 0x0

th_gdt_code:
    DW 0xFFFF    ; Segment length (bits 0-15)
    DW 0x0       ; Segment base   (bits 0-15)
    DB 0x0       ; Segment base   (bits 16-23)
    DB 10011010b ; Flags          (8 bits)
    DB 11001111b ; Flags          (4 bits) + segment length (bits 16-19)
    DB 0x0       ; Segment base   (bits 24-31)

th_gdt_data:
    DW 0xFFFF
    DW 0x0
    DB 0x0
    DB 10010010b
    DB 11001111b
    DB 0x0

th_gdt_end:

th_gdt_descriptor:
    DW th_gdt_end - th_gdt_start - 1
    DD th_gdt_start

TH_GDT_CODE_SEG equ th_gdt_code - th_gdt_start
TH_GDT_DATA_SEG equ th_gdt_data - th_gdt_start