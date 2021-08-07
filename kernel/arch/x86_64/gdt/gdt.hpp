#include <stdint.h>

#ifndef TH_KERN_GDT
#define TH_KERN_GDT

extern "C" {
    #include "thornhill_gdt.h"
}

extern "C" void* load_gdt(void* gdt_ptr);

class ThornhillGDT {
    private:
      static void memzero(void* memory, uint64_t length);

    public:
      static void setup();
};

#endif