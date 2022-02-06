#include "drivers/graphics.hpp"

#include "kernel/arch/x86_64/include.hpp"

#pragma once

#define PIT_FREQUENCY 1193180

typedef void (*ThornhillPITHandler)(ThornhillSystemTime* time);

/**
 * Kernel driver for the Intel 8253 programmable interval timer (PIT).
 *
 * https://en.wikipedia.org/wiki/Programmable_interval_timer
 * https://wiki.osdev.org/Programmable_Interval_Timer
 *
 */
class ThornhillPITDriver {
  private:
    static uint16_t tick;
    static ThornhillSystemTime currentTime;
    static ThornhillPITHandler onTimerCallbackHandler;

    static void triggerTimerCallback(interrupt_state_t);

  public:
    /**
     * Initializes the PIT driver, an interface for a legacy interrupt timer chip.
     * The frequency denominator (the first parameter) is the value that the maximum PIT frequency,
     * PIT_FREQUENCY (= 1,193,180) is divided by to give the frequency at which interrupts should
     * be generated in Hz.
     *
     * Refer to the OSDev article on the PIT for details on why the maximum value of the PIT is
     * as such. (Here, it's rounded from the true value of 1.193181666... Mhz).
     *
     * @param frequency The denominator of the frequency divisor.
     * @param startupTime The system startup time.
     */
    static void initialize(uint16_t frequency, ThornhillSystemTime startupTime);

    /**
     * Sets the handler for when the timer callback is triggered by the PIT.
     * @param handler The new handler for a timer callback.
     */
    static void setOnTimerCallback(ThornhillPITHandler handler);
};
