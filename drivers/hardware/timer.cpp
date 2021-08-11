#include "timer.hpp"

#include "../io.hpp"

void ThornhillTimer::timerCallback(interrupt_state_t) {
    if (updating)
        return;

    if (tick > 5) {
        updating = true;

        currentTime = ThornhillClock::readOfflineTime();
        tick = 0;

        updating = false;
    }

    ThornhillGraphics::drawTime(&currentTime);
    tick++;
};

void ThornhillTimer::initialize(uint16_t frequency, ThornhillSystemTime startupTime) {
    currentTime = startupTime;
    ThornhillInterrupt::registerInterruptHandler(IRQ0, timerCallback);

    uint16_t divisor = PIT_FREQUENCY / frequency;
    uint8_t low = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)(divisor >> 8);

    ThornhillIO::writeByteToPort(0x43, 0b00110110); // Command Port
    ThornhillIO::writeByteToPort(0x40, low);
    ThornhillIO::writeByteToPort(0x40, high);
}

bool ThornhillTimer::updating = false;
uint16_t ThornhillTimer::tick = 0;
ThornhillSystemTime ThornhillTimer::currentTime;