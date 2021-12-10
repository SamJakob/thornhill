#include "gdt.hpp"
#include <thornhill>

using namespace Thornhill;

void ThornhillGDT::setup() {
    memzero((size_t*) &tss, sizeof(tss));
    uint64_t tss_base = ((uint64_t)&tss);

    thornhill_gdt_table.tss_low.base15_0 = tss_base & 0xffff;
    thornhill_gdt_table.tss_low.base23_16 = (tss_base >> 16) & 0xff;
    thornhill_gdt_table.tss_low.base31_24 = (tss_base >> 24) & 0xff;
    thornhill_gdt_table.tss_low.limit15_0 = sizeof(tss);
    thornhill_gdt_table.tss_high.limit15_0 = (tss_base >> 32) & 0xffff;

    table_ptr gdt_ptr = {
        .limit = sizeof(thornhill_gdt_table) - 1,
        .base = (uint64_t)&thornhill_gdt_table
    };

    load_gdt(&gdt_ptr);
    Kernel::debug("GDT", "Registered Thornhill GDT successfully.");
}