#include "drivers/graphics.hpp"

#include "kernel/arch/x86_64/include.hpp"

#pragma once

#define PIT_FREQUENCY 1193180

class ThornhillTimer {
  private:
    static uint16_t tick;
    static ThornhillSystemTime currentTime;
    static void (*handler)(ThornhillSystemTime* currentTime);

    static void timerCallback(interrupt_state_t);

  public:
    static void initialize(uint16_t frequency, ThornhillSystemTime startupTime);
    static void setHandler(void (*handler)(ThornhillSystemTime* currentTime));
};
