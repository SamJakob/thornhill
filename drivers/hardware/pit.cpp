#include "pit.hpp"

void ThornhillPITDriver::triggerTimerCallback(interrupt_state_t) {
    if (tick > 5) {
        currentTime = ThornhillClock::readOfflineTime();
        ThornhillGraphicsDriver::drawTime(&currentTime);
	if (onTimerCallbackHandler != nullptr)
            onTimerCallbackHandler(&currentTime);
        tick = 0;
    }

    tick++;
}

void ThornhillPITDriver::setOnTimerCallback(ThornhillPITHandler handler) {
    ThornhillPITDriver::onTimerCallbackHandler = handler;
}

void ThornhillPITDriver::initialize(uint16_t frequency, ThornhillSystemTime startupTime) {
    currentTime = startupTime;
    ThornhillInterrupt::registerInterruptHandler(IRQ0, triggerTimerCallback);

    uint16_t divisor = PIT_FREQUENCY / frequency;
    auto low = (uint8_t)(divisor & 0xFF);
    auto high = (uint8_t)(divisor >> 8);

    ThornhillIODriver::writeByteToPort(0x43, 0b00110110); // Command Port
    ThornhillIODriver::writeByteToPort(0x40, low);
    ThornhillIODriver::writeByteToPort(0x40, high);
}

uint16_t ThornhillPITDriver::tick = 0;
ThornhillSystemTime ThornhillPITDriver::currentTime;
ThornhillPITHandler ThornhillPITDriver::onTimerCallbackHandler = nullptr;
