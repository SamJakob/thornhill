#include "keyboard.hpp"

void ThornhillKeyboard::initialize() {
    ThornhillInterrupt::registerInterruptHandler(IRQ1, ThornhillKeyboard::handleInterrupt);
}

uint16_t xOffset = 0;
uint16_t yOffset = 0;

void ThornhillKeyboard::handleInterrupt(interrupt_state_t) {
    uint8_t keycode = ThornhillIO::readByteFromPort(0x60);

    if (keycode > KEYCODE_MAX)
        return;

    if (keycode == KEY_ENTER) {
      yOffset += 10;
      xOffset = 0;
      return;
    }

    if (keycode == KEY_SPACE) {
      xOffset +=10;
      return;
    }

    if (keycode == KEY_BACKSPACE) {
        xOffset -= 10;
        ThornhillGraphics::drawRect(rgb(0,0,0), 130 + xOffset, 160 + yOffset, 10, 10);
        return;
    }
      
    char letter = THKeyboard::keycode_ascii[(int)keycode];
    ThornhillGraphics::drawCharacter(letter, 130 + xOffset, 160 + yOffset);
    xOffset+=10;
}
