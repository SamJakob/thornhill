#include <stdint.h>

#ifndef TH_KERN_GDT
#define TH_KERN_GDT

extern "C" {
    #include "thornhill_gdt.h"
}

extern "C" void* load_gdt(void* gdt_ptr);

class ThornhillGDT {
    private:
        static void memzero(void* memory, uint64_t length) {
            for (uint64_t i = 0; i < length; i++) ((uint8_t*) memory)[i] = 0;
        }

    public:
        static void setup() {
            memzero((void*) &tss, sizeof(tss));
            uint64_t tss_base = ((uint64_t) &tss);

            thornhill_gdt_table.tss_low.base15_0 = tss_base & 0xffff;
            thornhill_gdt_table.tss_low.base23_16 = (tss_base >> 16) & 0xff;
            thornhill_gdt_table.tss_low.base31_24 = (tss_base >> 24) & 0xff;
            thornhill_gdt_table.tss_low.limit15_0 = sizeof(tss);
            thornhill_gdt_table.tss_high.limit15_0 = (tss_base >> 32) & 0xffff;

            table_ptr gdt_ptr = { sizeof(thornhill_gdt_table) - 1, (uint64_t) &thornhill_gdt_table };
            load_gdt(&gdt_ptr);
        }
};

#endif