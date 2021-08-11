#include <stdint.h>

#ifndef TH_KERNEL_ARCH_GDT
#define TH_KERNEL_ARCH_GDT

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