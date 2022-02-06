#include "kernel/keyboard/keyboard.hpp"

#include "../io.hpp"

#include "kernel/arch/x86_64/include.hpp"

#pragma once

/**
 * Kernel driver for the hardware keyboard. This driver is responsible for passing keyboard
 * input to a unified interface in the kernel for accepting keyboard input.
 */
class ThornhillKeyboardDriver {

    public:
        static void initialize();
        static void handleInterrupt(interrupt_state_t);

};
