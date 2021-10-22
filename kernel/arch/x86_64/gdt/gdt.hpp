#pragma once

extern "C" {
  #include "thornhill_gdt.h"
}

extern "C" void* load_gdt(void* gdt_ptr);

class ThornhillGDT {
    public:
      static void setup();
};
