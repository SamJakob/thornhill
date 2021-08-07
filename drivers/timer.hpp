#include "../kernel/utils.hpp"
#include "graphics.hpp"

#include "kernel/arch/x86_64/include.hpp"

#include <stdint.h>

#ifndef TH_DRIVER_TIMER
#define TH_DRIVER_TIMER

#define PIT_FREQUENCY 1193180

class ThornhillTimer {
  private:
    static bool updating;
    static uint16_t tick;
    static ThornhillSystemTime currentTime;

    static void timerCallback(interrupt_state_t);

  public:
    static void initialize(uint16_t frequency, ThornhillSystemTime startupTime);
};

#endif