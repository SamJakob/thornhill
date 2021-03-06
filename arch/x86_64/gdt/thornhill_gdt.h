#ifndef TH_KERN_GDT_H
#define TH_KERN_GDT_H

#define TH_KERNEL_CODE_SEGMENT 0x08
#define TH_KERNEL_DATA_SEGMENT 0x10

#pragma pack (1)

struct gdt_descriptor {
    uint16_t limit15_0;
    uint16_t base15_0;

    uint8_t base23_16;
    uint8_t type;

    uint8_t limit19_16_and_flags;
    uint8_t base31_24;
};

struct tss {
    uint32_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iopb_offset;
} tss;

__attribute__((aligned(4096)))
struct {
    struct gdt_descriptor null;
    struct gdt_descriptor kernel_code;
    struct gdt_descriptor kernel_data;
    struct gdt_descriptor null2;
    struct gdt_descriptor user_data;
    struct gdt_descriptor user_code;
    struct gdt_descriptor ovmf_data;
    struct gdt_descriptor ovmf_code;
    struct gdt_descriptor tss_low;
    struct gdt_descriptor tss_high;
} thornhill_gdt_table = {
    {0, 0, 0, 0x00, 0x00, 0},   /* = (0x00): null */
    {0, 0, 0, 0x9a, 0xa0, 0},   /* = (0x08): kernel code (kernel base selector) */
    {0, 0, 0, 0x92, 0xa0, 0},   /* = (0x10): kernel data */
    {0, 0, 0, 0x00, 0x00, 0},   /* = (0x18): null (user base selector) */
    {0, 0, 0, 0x92, 0xa0, 0},   /* = (0x20): user data */
    {0, 0, 0, 0x9a, 0xa0, 0},   /* = (0x28): user code */
    {0, 0, 0, 0x92, 0xa0, 0},   /* = (0x30): ovmf data */
    {0, 0, 0, 0x9a, 0xa0, 0},   /* = (0x38): ovmf code */
    {0, 0, 0, 0x89, 0xa0, 0},   /* = (0x40): tss low */
    {0, 0, 0, 0x00, 0x00, 0},   /* = (0x48): tss high */
};

struct table_ptr {
    uint16_t limit;
    uint64_t base;
};

#pragma pack ()

#endif