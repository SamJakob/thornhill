extern "C" {
#include "boot/handoff/handoff_shared.h"
}

#include "lib/thornhill.hpp"

#include "drivers/clock.hpp"
#include "drivers/graphics.hpp"
#include "drivers/hardware/keyboard.hpp"
#include "drivers/io.hpp"
#include "drivers/timer.hpp"

#include "memory/manager.hpp"
#include "memory/physical.hpp"

using namespace Thornhill;
// #include "../lib/posix.hpp"
// using namespace POSIX;

void main(ThornhillHandoff* thornhillHandoff) {

    // Read startup time.
    ThornhillSystemTime startupTime = ThornhillClock::readOfflineTime();

    // Start memory management.
    ThornhillMemoryManager::initialize(thornhillHandoff->memoryMap);

    /** INITIAL SETUP **/

    // Initialize the display and timer.
    ThornhillGraphics::initialize(thornhillHandoff->screen);

    // Register the new interrupt handlers.
    ThornhillInterrupt::setupInterrupts();
    ThornhillInterrupt::setAllowInterrupts(true);

    // Register the keyboard driver.
    ThornhillKeyboard::initialize();

    /** KERNEL **/
    ThornhillTimer::initialize(20, startupTime);
    ThornhillGraphics::drawTTY();

    // sprintf(buf, "%s", "hello");
    // ThornhillGraphics::drawText(buf, 130, 150);

    ThornhillGraphics::drawTime(&startupTime);
}

extern "C" [[noreturn]] void _start(ThornhillHandoff* thornhillHandoff) {

    ThornhillInterrupt::setAllowInterrupts(false);
    ThornhillGDT::setup();

    ThornhillMemory::Physical::reset();

    main(thornhillHandoff);
    for (;;) {}
    
}

void Kernel::panic(const char* reason, uint64_t interruptNumber) {

    ThornhillGraphics::clear(rgb(34, 34, 34));

    ThornhillGraphics::drawText("Thornhill", 20, 50, 6);
    ThornhillGraphics::drawText("// The system needs to be restarted.", 20, 100, 2);

    if (interruptNumber != 69) {
        ThornhillGraphics::drawText(THUtils::int_to_ascii(interruptNumber), 20, 150, 2);
        ThornhillGraphics::drawText(reason, 20, 170, 2);
    } else {
        ThornhillGraphics::drawText(reason, 20, 150, 2);
    }

    // For now, halt upon getting a kernel panic.
    for (;;) {
    }
}

extern "C" void interrupt_handler(interrupt_state_t interruptState) {

    Kernel::panic(exceptionMessages[interruptState.int_no], interruptState.int_no);
}

extern "C" void interrupt_request_handler(interrupt_state_t interruptState) {
    // After receiving an interrupt request, an EOI (End-Of-Interrupt) needs
    // to be sent to the PICs to indicate that new interrupts may be sent.
    if (interruptState.int_no >= 40)
        ThornhillIO::writeByteToPort(0xA0, 0x20); // slave
    ThornhillIO::writeByteToPort(0x20, 0x20);     // master

    if (ThornhillInterrupt::hasHandlerFor(interruptState.int_no)) {
        interrupt_handler_t handler = ThornhillInterrupt::getHandlerFor(interruptState.int_no);
        handler(interruptState);
    }
}