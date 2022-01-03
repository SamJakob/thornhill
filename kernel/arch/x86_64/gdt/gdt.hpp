#include "thornhill_gdt.h"

#pragma once

extern "C" void* load_gdt(void* gdt_ptr);

class ThornhillGDT {
    public:
      static void setup();
};
