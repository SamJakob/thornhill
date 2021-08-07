#include "thornhill_gdt.h"

#pragma pack (1)

struct tss tss;

struct thornhill_gdt_table thornhill_gdt_table = {
    {0, 0, 0, 0x00, 0x00, 0}, /* = (0x00): null */
    {0, 0, 0, 0x9a, 0xa0, 0}, /* = (0x08): kernel code (kernel base selector) */
    {0, 0, 0, 0x92, 0xa0, 0}, /* = (0x10): kernel data */
    {0, 0, 0, 0x00, 0x00, 0}, /* = (0x18): null (user base selector) */
    {0, 0, 0, 0x92, 0xa0, 0}, /* = (0x20): user data */
    {0, 0, 0, 0x9a, 0xa0, 0}, /* = (0x28): user code */
    {0, 0, 0, 0x92, 0xa0, 0}, /* = (0x30): ovmf data */
    {0, 0, 0, 0x9a, 0xa0, 0}, /* = (0x38): ovmf code */
    {0, 0, 0, 0x89, 0xa0, 0}, /* = (0x40): tss low */
    {0, 0, 0, 0x00, 0x00, 0}, /* = (0x48): tss high */
};

#pragma pack()