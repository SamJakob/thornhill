#include <stdint.h>

#ifndef TH_KERNEL_ARCH_IDT_H
#define TH_KERNEL_ARCH_IDT_H

#pragma pack (1)

#define offset15_0(address) (uint16_t)((address) & 0xFFFF);
#define offset31_16(address) (uint16_t)(((address) >> 16) & 0xFFFF);
#define offset63_32(address) (uint32_t)(((address) >> 32) & 0xFFFFFFFF);

typedef struct {
    uint16_t offset15_0;
    uint16_t selector;
    uint8_t null_descriptor;
    uint8_t type;
    uint16_t offset31_16;
    uint32_t offset63_32;
    uint32_t null_descriptor_2;
} idt_descriptor_t;

typedef struct {
    uint16_t limit;
    uint64_t base;
} idt_pointer_t;

#define IDT_ENTRIES 256
extern idt_descriptor_t idt[IDT_ENTRIES];
extern idt_pointer_t idt_register;

#pragma pack ()

typedef enum {
    IDT_GATE_32BIT_TASK     = 0b0101,
    IDT_GATE_16BIT_INT      = 0b0110,
    IDT_GATE_16BIT_TRAP     = 0b0111,
    IDT_GATE_32BIT_INT      = 0b1110,
    IDT_GATE_32BIT_TRAP     = 0b1111
} CpuIdtGateType;

#endif