#include "keyboard.hpp"

void (*handler)(uint8_t);

void ThornhillKeyboard::initialize() {
    handler = &ThornhillKeyboard::DefaultHandler;
    ThornhillInterrupt::registerInterruptHandler(IRQ1, ThornhillKeyboard::handleInterrupt);
}

void ThornhillKeyboard::setHandler(void (*handlerIn)(uint8_t)) {
  if (handlerIn == nullptr) handler = &ThornhillKeyboard::DefaultHandler;
  else handler = handlerIn;
}

uint16_t xOffset = 0;
uint16_t yOffset = 0;

void ThornhillKeyboard::DefaultHandler(uint8_t keycode) {
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
        if (xOffset != 0)
            xOffset -= 10;
        ThornhillGraphics::drawRect(rgb(0,0,0), 130 + xOffset, 160 + yOffset, 10, 10);
        return;
    }
      
    char letter = THKeyboard::keycode_ascii[(int)keycode];
    ThornhillGraphics::drawCharacter(letter, 130 + xOffset, 160 + yOffset);
    xOffset+=10;
}

void ThornhillKeyboard::handleInterrupt(interrupt_state_t) {
    uint8_t keycode = ThornhillIO::readByteFromPort(0x60);

    if (handler != nullptr)
      handler(keycode);
}
