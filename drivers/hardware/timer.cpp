#include "timer.hpp"

void ThornhillTimer::timerCallback(interrupt_state_t) {
    if (tick > 5) {
        currentTime = ThornhillClock::readOfflineTime();
        ThornhillGraphics::drawTime(&currentTime);
	if (handler != nullptr) handler(&currentTime);
        tick = 0;
    }

    tick++;
}

void ThornhillTimer::setHandler(void (*handler)(ThornhillSystemTime*)) {
    ThornhillTimer::handler = handler;
}

void ThornhillTimer::initialize(uint16_t frequency, ThornhillSystemTime startupTime) {
    currentTime = startupTime;
    ThornhillInterrupt::registerInterruptHandler(IRQ0, timerCallback);

    uint16_t divisor = PIT_FREQUENCY / frequency;
    auto low = (uint8_t)(divisor & 0xFF);
    auto high = (uint8_t)(divisor >> 8);

    ThornhillIO::writeByteToPort(0x43, 0b00110110); // Command Port
    ThornhillIO::writeByteToPort(0x40, low);
    ThornhillIO::writeByteToPort(0x40, high);
}

uint16_t ThornhillTimer::tick = 0;
ThornhillSystemTime ThornhillTimer::currentTime;
void (*ThornhillTimer::handler)(ThornhillSystemTime*) = nullptr;
