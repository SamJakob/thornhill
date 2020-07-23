#include <stdint.h>
#include "graphics.cpp"
#include "io.cpp"
#include "../kernel/interrupt/interrupt.cpp"
#include "../kernel/utils.cpp"

#ifndef TH_DRIVER_TIMER
#define TH_DRIVER_TIMER

#define PIT_FREQUENCY 1193180

class ThornhillTimer {
    private:
        static bool updating;
        static uint16_t tick;
        static ThornhillSystemTime currentTime;

        static void timerCallback(interrupt_state_t) {
            if (updating) return;

            if (tick > 5) {
                updating = true;

                currentTime = ThornhillClock::readOfflineTime();
                tick = 0;

                updating = false;
            }

            ThornhillGraphics::drawStatusBar(&currentTime);
            tick++;
        }

    public:
        static void initialize(uint16_t frequency, ThornhillSystemTime startupTime) {
            currentTime = startupTime;
            ThornhillInterrupt::registerInterruptHandler(IRQ0, timerCallback);

            uint16_t divisor = PIT_FREQUENCY / frequency;
            uint8_t low = (uint8_t)(divisor & 0xFF);
            uint8_t high = (uint8_t)(divisor >> 8);

            ThornhillIO::writeByteToPort(0x43, 0b00110110); // Command Port
            ThornhillIO::writeByteToPort(0x40, low);
            ThornhillIO::writeByteToPort(0x40, high);
        }
};

bool ThornhillTimer::updating = false;
uint16_t ThornhillTimer::tick = 0;
ThornhillSystemTime ThornhillTimer::currentTime;

#endif