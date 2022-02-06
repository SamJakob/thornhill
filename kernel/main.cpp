#include <thornhill>

#include "boot/handoff/handoff_shared.h"
#include "drivers/clock.hpp"
#include "drivers/graphics.hpp"
#include "drivers/io.hpp"
#include "drivers/hardware/keyboard.hpp"
#include "drivers/hardware/pit.hpp"
#include "drivers/hardware/serial.hpp"
#include "ski/ski.hpp"

#include "memory/physical.hpp"

using namespace Thornhill;

bool HAS_BOOTED = false;

void main(ThornhillHandoff* thornhillHandoff) {

    /** SYSTEM STARTUP **/

    // Read startup time.
    ThornhillSystemTime startupTime = ThornhillClock::readOfflineTime();

    // Start memory management.
    ThornhillMemory::Physical::initialize(thornhillHandoff->memoryMap);

    // Register the new interrupt handlers.
    ThornhillInterrupt::setupInterrupts();
    ThornhillInterrupt::setAllowInterrupts(true);

    // Register the keyboard driver.
    ThornhillKeyboard::setOnKeyPress(&ThornhillSKI::handleInput);
    ThornhillKeyboardDriver::initialize();

    /** KERNEL **/
    HAS_BOOTED = true;
    ThornhillPITDriver::initialize(20, startupTime);
    ThornhillGraphicsDriver::drawTTY();

    ThornhillGraphicsDriver::drawTime(&startupTime);

    ThornhillSKI::initialize();
    ThornhillPITDriver::setOnTimerCallback(&ThornhillSKI::handleTimer);

    Kernel::print("System is ready.");

    //ThornhillMemory::Physical::allocate(5);
    //ThornhillMemory::Physical::allocate(3);
}

extern "C" [[maybe_unused]] [[noreturn]] void _start(ThornhillHandoff* thornhillHandoff) {

    HAS_BOOTED = false;
    ThornhillInterrupt::setAllowInterrupts(false);

    // Initialize the serial and display drivers.
    ThornhillSerialDriver::initialize();
    Kernel::debug("Initializing kernel core...");

    ThornhillGraphicsDriver::initialize(thornhillHandoff->screen);

    // Initialize core memory management services.
    ThornhillGDT::setup();
    ThornhillMemory::Physical::reset();

    main(thornhillHandoff);
    for (;;) {}

}

void Kernel::panic(const char* reason, uint64_t interruptNumber, const char* context) {

    ThornhillInterrupt::setAllowInterrupts(false);

    ThornhillSerialDriver::write("!!! THORNHILL KERNEL PANIC !!!");
    ThornhillSerialDriver::write(reason);
    if (context != nullptr)
        ThornhillSerialDriver::write(context);
    ThornhillSerialDriver::write("!!! THORNHILL KERNEL PANIC !!!");

    ThornhillGraphicsDriver::clear(rgb(34, 34, 34));

    ThornhillGraphicsDriver::drawText("Thornhill", 20, 50, 6);
    ThornhillGraphicsDriver::drawText(
        HAS_BOOTED ? "Your system needs to be restarted." : "Your system failed to boot.",
        20, 100, 2
    );

    if (interruptNumber == SYSTEM_SELF_PANIC_INTERRUPT_NUMBER) {
        /* A kernel software error triggered the panic. */

        // Print the reason and context (if available) below the reason.
        ThornhillGraphicsDriver::drawText(reason, 20, 150, 2);
        ThornhillGraphicsDriver::drawText(context, 20, 170, 1);
    } else {
        /* A hardware error caused the kernel panic. */

        // Temporary buffer for converting the interrupt number to a string.
        char itoaBuffer[6];

        // Draw the interrupt number and reason to the screen.
        ThornhillGraphicsDriver::drawText(uitoa(itoaBuffer, interruptNumber, 10, 6), 20, 150, 2);
        ThornhillGraphicsDriver::drawText(reason, 20, 170, 2);
    }

    // For now, halt upon getting a kernel panic.
    for (;;) {}

}

extern "C" [[maybe_unused]] void interrupt_exception_handler(interrupt_state_t interruptState) {
    switch (interruptState.int_no) {
        // Page Fault
        case 14:
            Kernel::print("Page fault occurred.");
            Kernel::printf("Accessed address: %x%n", interruptState.cr2);

            [[fallthrough]];

        default:
            Kernel::panic(exceptionMessages[interruptState.int_no], interruptState.int_no);
    }
}

extern "C" [[maybe_unused]] void interrupt_request_handler(interrupt_state_t interruptState) {
    // After receiving an interrupt request, an EOI (End-Of-Interrupt) needs
    // to be sent to the PICs to indicate that new interrupts may be sent.
    if (interruptState.int_no >= 40)
        ThornhillIODriver::writeByteToPort(0xA0, 0x20); // slave
    ThornhillIODriver::writeByteToPort(0x20, 0x20);     // master

    if (ThornhillInterrupt::hasHandlerFor(interruptState.int_no)) {
        interrupt_handler_t handler = ThornhillInterrupt::getHandlerFor(interruptState.int_no);
        handler(interruptState);
    }
}

// Not currently randomized. Just implemented for bug checking.
#define STACK_CHK_GUARD 0x5fbb6beef86cd9f4
[[maybe_unused]] uintptr_t __stack_chk_guard = STACK_CHK_GUARD; // NOLINT(bugprone-reserved-identifier)

extern "C" [[maybe_unused]] [[noreturn]] void __stack_chk_fail(void) { // NOLINT(bugprone-reserved-identifier)
    Kernel::panic("Stack check failure.");
}
