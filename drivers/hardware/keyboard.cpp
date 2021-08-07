#include "keyboard.hpp"

void ThornhillKeyboard::initialize() {
    ThornhillInterrupt::registerInterruptHandler(IRQ1, ThornhillKeyboard::handleInterrupt);
}

void ThornhillKeyboard::handleInterrupt(interrupt_state_t) {
    uint8_t keycode = ThornhillIO::readByteFromPort(0x60);

    if (keycode > KEYCODE_MAX)
        return;

    char letter = THKeyboard::keycode_ascii[(int)keycode];
    ThornhillGraphics::drawCharacter(letter, 130, 160);
}