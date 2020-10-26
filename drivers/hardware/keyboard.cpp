#include "kernel/keyboard/keyboard.h"

#include "../io.hpp"
#include "../graphics.cpp"

#ifndef THORNHILL_DRIVER_HARDWARE_KEYBOARD
#define THORNHILL_DRIVER_HARDWARE_KEYBOARD

class ThornhillKeyboard {

    // private:
    //    static char buf[];

    public:
        static void initialize() {
            ThornhillInterrupt::registerInterruptHandler(IRQ1, ThornhillKeyboard::handleInterrupt);
        }

        static void handleInterrupt(interrupt_state_t) {
            uint8_t keycode = ThornhillIO::readByteFromPort(0x60);

            if (keycode > KEYCODE_MAX) return;

            char letter = THKeyboard::keycode_ascii[(int) keycode];
            ThornhillGraphics::drawCharacter(letter, 130, 160);
        }

};

#endif