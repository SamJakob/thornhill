extern "C" {
    #include "boot/handoff.h"
}

#include "arch/x86_64/include.cpp"

#include "drivers/io.cpp"
#include "drivers/clock.cpp"
#include "drivers/graphics.cpp"
#include "drivers/timer.cpp"
#include "drivers/hardware/keyboard.cpp"

#include "interrupt/interrupt.cpp"
#include "utils.cpp"

#include "memory/physical.cpp"

#include "lib/thornhill.cpp"
using namespace Thornhill;
// #include "../lib/posix.cpp"
// using namespace POSIX;

void main(ThornhillHandoff* thornhillHandoff) {

    // Read startup time.
    ThornhillSystemTime startupTime = ThornhillClock::readOfflineTime();

    /** INITIAL SETUP **/

    // Initialize the display and timer.
    ThornhillGraphics::initialize(thornhillHandoff->screen);

    // Register the new interrupt handlers.
    ThornhillInterrupt::setupInterrupts();

    // Re-enable interrupts.
    ThornhillInterrupt::setAllowInterrupts(true);

    /*
    // Test memory.
    int* pointer = 0;

    for (unsigned int i = 0; i <= 0xBFFFFFFF; i++) {
        (*pointer)++;
        pointer++;
    }
    */

    // Draw the interface.
    ThornhillGraphics::drawStatusBar(&startupTime);

    // Register the keyboard driver.
    ThornhillKeyboard::initialize();


    /** KERNEL **/
    ThornhillTimer::initialize(20, startupTime);
    ThornhillGraphics::drawTTY();

    // sprintf(buf, "%s", "hello");
    // ThornhillGraphics::drawText(buf, 130, 150);

}

extern "C" void _start(
    ThornhillHandoff* thornhillHandoff
) {
    
    ThornhillInterrupt::setAllowInterrupts(false);
    ThornhillGDT::setup();
    
    main(thornhillHandoff);
    for(;;) {}

}

void Kernel::panic(const char* reason, uint64_t interruptNumber) {

    ThornhillGraphics::clear(rgb(34, 34, 34));

    ThornhillGraphics::drawText("Thornhill", 20, 50, 6);
    ThornhillGraphics::drawText("// Your computer needs to be restarted.", 20, 100, 2);


    ThornhillGraphics::drawText(THUtils::int_to_ascii(interruptNumber), 20, 150, 2);
    ThornhillGraphics::drawText(reason, 20, 170, 2);


    // For now, halt upon getting a kernel panic.
    for(;;) {}

}

extern "C" void interrupt_handler(interrupt_state_t interruptState) {

    Kernel::panic(exceptionMessages[interruptState.int_no], interruptState.int_no);

}

extern "C" void interrupt_request_handler(interrupt_state_t interruptState) {
    // After receiving an interrupt request, an EOI (End-Of-Interrupt) needs
    // to be sent to the PICs to indicate that new interrupts may be sent.
    if (interruptState.int_no >= 40) ThornhillIO::writeByteToPort(0xA0, 0x20); // slave
    ThornhillIO::writeByteToPort(0x20, 0x20); // master

    if (ThornhillInterrupt::hasHandlerFor(interruptState.int_no)) {
        interrupt_handler_t handler = ThornhillInterrupt::getHandlerFor(interruptState.int_no);
        handler(interruptState);
    }
}