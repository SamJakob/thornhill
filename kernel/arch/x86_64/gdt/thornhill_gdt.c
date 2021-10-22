#include "thornhill_gdt.h"

#pragma pack (1)

struct tss tss;

struct thornhill_gdt_table thornhill_gdt_table = {
    /*   base,                      limit,                 flag */                  // table offset: purpose
    /*----------------------------------------------------------*/                  // ---------------------
    gdtd(0,                         0,                     0),                      // 0x00:         null
    gdtd(0,                         0,                     TH_GDTD_CODE),           // 0x08:         kernel code
    gdtd(0,                         0,                     TH_GDTD_DATA),           // 0x10:         kernel data
    gdtd(0,                         0,                     0),                      // 0x18:         null (user base selector)
    gdtd(0,                         0,                     TH_GDTD_DATA),           // 0x20:         user data
    gdtd(0,                         0,                     TH_GDTD_CODE),           // 0x28:         user code
    gdtd(0,                         0,                     TH_GDTD_DATA),           // 0x30:         firmware data
    gdtd(0,                         0,                     TH_GDTD_CODE),           // 0x38:         firmware code
    gdtd(0,                         0,                     TH_GDTD_TSS),            // 0x40:         tss low
    gdtd(0,                         0,                     0),                      // 0x48:         tss high
};

// Reminder:
// We're in long mode, so limit is ignored for all segments and base is
// ignored for all except fs and gs. Where ignored, it's expected these
// values be initialized to 0.

#pragma pack()