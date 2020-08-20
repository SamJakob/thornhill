extern "C" {
    #include "../boot/handoff.h"
}

#include "../drivers/clock.cpp"
#include "../drivers/graphics.cpp"
#include "../drivers/timer.cpp"
#include "memory/gdt.cpp"
#include "interrupt/interrupt.cpp"
#include "utils.cpp"

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

    // Draw the interface.
    ThornhillGraphics::drawStatusBar(&startupTime);


    /** KERNEL **/
    ThornhillTimer::initialize(20, startupTime);

}

extern "C" void _start(
    ThornhillHandoff* thornhillHandoff
) {
    
    ThornhillInterrupt::setAllowInterrupts(false);
    ThornhillGDT::setup();
    
    main(thornhillHandoff);
    for(;;) {}

}

extern "C" void interrupt_handler(interrupt_state_t interruptState) {

    ThornhillGraphics::clear(rgb(34, 34, 34));

    ThornhillGraphics::drawText("Thornhill", 20, 50, 6);
    ThornhillGraphics::drawText("// Your computer needs to be restarted.", 20, 100, 2);
    
    ThornhillGraphics::drawText(ThornhillUtils::int_to_ascii(interruptState.int_no), 20, 150, 2);
    ThornhillGraphics::drawText(exceptionMessages[interruptState.int_no], 20, 170, 2);

    // For now, halt upon getting a CPU interrupt.
    for(;;) {}

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