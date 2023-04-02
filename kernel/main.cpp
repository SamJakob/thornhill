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
#include "kernel/memory/virtual.hpp"

#include "config/version.h"
#include "cstring"

using namespace Thornhill;

// BEGIN DEBUG

extern "C" [[maybe_unused]] uint32_t TH_DEBUGGER_SENTINEL;
[[maybe_unused]] uint32_t TH_DEBUGGER_SENTINEL = 0x534A4142;

extern "C" [[maybe_unused]] uint32_t volatile TH_VERSION_IDENTIFIER_LEN;
[[maybe_unused]] uint32_t volatile TH_VERSION_IDENTIFIER_LEN = 0;

extern "C" [[maybe_unused]] uintptr_t TH_VERSION_IDENTIFIER;
[[maybe_unused]] uintptr_t TH_VERSION_IDENTIFIER = 0;

// Temporarily set here. TODO: cleanup
const char* TH_VERSION_IDENTIFIER_STRING = "Thornhill - build " TH_GIT_REV " (built on " TH_BUILD_DATE " at " TH_BUILD_TIME ")\n";

// END DEBUG

bool HAS_BOOTED = false;

void main() {

    /** SYSTEM STARTUP **/

    // Read startup time.
    ThornhillSystemTime startupTime = ThornhillClock::readOfflineTime();

    // Re-enable interrupts.
    ThornhillInterrupt::setAllowInterrupts(true);

    // Register the keyboard driver.
    ThornhillKeyboard::setOnKeyPress(ThornhillSKI::handleInput);
    ThornhillKeyboardDriver::initialize();

    /** KERNEL **/
    HAS_BOOTED = true;

    TH_VERSION_IDENTIFIER = (uintptr_t) TH_VERSION_IDENTIFIER_STRING;
    TH_VERSION_IDENTIFIER_LEN = strlen(TH_VERSION_IDENTIFIER_STRING);

    ThornhillPITDriver::initialize(20, startupTime);
    ThornhillGraphicsDriver::drawTTY();

    ThornhillGraphicsDriver::drawTime(&startupTime);

    ThornhillSKI::initialize();
    ThornhillPITDriver::setOnTimerCallback(ThornhillSKI::handleTimer);

    Kernel::print("System is ready.");

    void* memA = ThornhillMemory::Physical::allocate(9);
    void* memB = ThornhillMemory::Physical::allocate(3);

    ThornhillMemory::Physical::deallocate(memA, 9);
    ThornhillMemory::Physical::deallocate(memB, 3);
}

extern "C" [[maybe_unused]] [[noreturn]] void _start(ThornhillHandoff* handoff) {

    HAS_BOOTED = false;
    ThornhillInterrupt::setAllowInterrupts(false);

    // Initialize the serial and display drivers.
    ThornhillSerialDriver::initialize();
    Kernel::debug("Initializing kernel core...");

    // Enable the graphics driver.
    ThornhillGraphicsDriver::initialize(handoff->screen);

    // Set up the global descriptor table and interrupts.
    ThornhillGDT::setup();
    ThornhillInterrupt::setupInterrupts();

    // Initialize core memory management services.
    ThornhillMemory::Physical::reset();
    ThornhillMemory::Physical::initialize(handoff->memoryMap);
    ThornhillMemory::Virtual::reset();
    ThornhillMemory::Virtual::initialize();

    main();
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
        ThornhillInterrupt::triggerHandlerFor(interruptState.int_no, interruptState);
    }
}

// Not currently randomized. Just implemented for bug checking.
#define STACK_CHK_GUARD 0x5fbb6beef86cd9f4
[[maybe_unused]] uintptr_t __stack_chk_guard = STACK_CHK_GUARD; // NOLINT(bugprone-reserved-identifier)

extern "C" [[maybe_unused]] [[noreturn]] void __stack_chk_fail(void) { // NOLINT(bugprone-reserved-identifier)
    Kernel::panic("Stack check failure.");
}
