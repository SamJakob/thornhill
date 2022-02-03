#pragma once

#include "stdint.h"

#define ThornHillSKIBufferSize 2024

class ThornhillSKI {

    private:
    //   static char buffer[ThornHillSKIBufferSize];
    //   static uint16_t start;

    public:
      static void initialize();
      static void draw();
      static void handleInput(uint8_t);
      static void insertChar(char);
      static void insert(const char* str);
      static void goBack();
      static void insertCharAd(char, bool check, bool draw);
      static void process();
};
