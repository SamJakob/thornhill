#include "keyboard.hpp"

void ThornhillKeyboardDriver::initialize() {
    ThornhillInterrupt::registerInterruptHandler(
        IRQ1,
        ThornhillKeyboardDriver::handleInterrupt
    );
}

void ThornhillKeyboardDriver::handleInterrupt(interrupt_state_t) {
    uint8_t keycode = ThornhillIODriver::readByteFromPort(0x60);
    ThornhillKeyboard::triggerKeyPress(keycode);
}
