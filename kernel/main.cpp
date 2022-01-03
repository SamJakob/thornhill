extern "C" {
#include "boot/handoff/handoff_shared.h"
}

#include <thornhill>

#include "drivers/clock.hpp"
#include "drivers/graphics.hpp"
#include "drivers/io.hpp"
#include "drivers/hardware/keyboard.hpp"
#include "drivers/hardware/timer.hpp"
#include "drivers/hardware/serial.hpp"

#include "memory/physical.hpp"

using namespace Thornhill;
// #include "../lib/posix.hpp"
// using namespace POSIX;

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
    ThornhillKeyboard::initialize();

    /** KERNEL **/
    HAS_BOOTED = true;
    ThornhillTimer::initialize(20, startupTime);
    ThornhillGraphics::drawTTY();

    // sprintf(buf, "%s", "hello");
    // ThornhillGraphics::drawText(buf, 130, 150);

    ThornhillGraphics::drawTime(&startupTime);

//    const char* pagingTestMessage = "In the unlikely event you see this message, paging is actually working!";
//
//    auto* pagingTestMessageAddr = reinterpret_cast<uint8_t*>((uint8_t*) pagingTestMessage);
//    // Now subtract the paging base address from the test message,
//    // so we can access the same segment of physical memory from
//    // its identity-mapped address (rather than kernel-mapped
//    // virtual address).
//    pagingTestMessageAddr = pagingTestMessageAddr - 0x0000001000000000;
//
//    ThornhillGraphics::drawText((const char*) pagingTestMessageAddr, 130, 160);

//    TLB::flush();
//
//    uintptr_t badptr = 0xdeadbeef00;
//    int x = *((int*)badptr);
//
//    Kernel::printf("%x", x);

    Kernel::print("System is ready.");

    ThornhillMemory::Physical::allocate(5);
    ThornhillMemory::Physical::allocate(3);
}

extern "C" [[noreturn]] void _start(ThornhillHandoff* thornhillHandoff) {

    HAS_BOOTED = false;
    ThornhillInterrupt::setAllowInterrupts(false);

    // Initialize the serial and display drivers.
    ThornhillSerial::initialize();
    Kernel::debug("Initializing kernel core...");

    ThornhillGraphics::initialize(thornhillHandoff->screen);

    // Initialize core memory management services.
    ThornhillGDT::setup();
    ThornhillMemory::Physical::reset();

    main(thornhillHandoff);
    for (;;) {}

}

void Kernel::panic(const char* reason, uint64_t interruptNumber, const char* context) {

    ThornhillInterrupt::setAllowInterrupts(false);

    ThornhillSerial::write("!!! THORNHILL KERNEL PANIC !!!");
    ThornhillSerial::write(reason);
    if (context != nullptr) ThornhillSerial::write(context);
    ThornhillSerial::write("!!! THORNHILL KERNEL PANIC !!!");

    ThornhillGraphics::clear(rgb(34, 34, 34));

    ThornhillGraphics::drawText("Thornhill", 20, 50, 6);
    ThornhillGraphics::drawText(
        HAS_BOOTED ? "Your system needs to be restarted." : "Your system failed to boot.",
        20, 100, 2
    );

    if (interruptNumber == SYSTEM_SELF_PANIC_INTERRUPT_NUMBER) {
        /* A kernel software error triggered the panic. */

        // Print the reason and context (if available) below the reason.
        ThornhillGraphics::drawText(reason, 20, 150, 2);
        ThornhillGraphics::drawText(context, 20, 170, 1);
    } else {
        /* A hardware error caused the kernel panic. */

        // Temporary buffer for converting the interrupt number to a string.
        char itoaBuffer[6];

        // Draw the interrupt number and reason to the screen.
        ThornhillGraphics::drawText(uitoa(itoaBuffer, interruptNumber, 10, 6), 20, 150, 2);
        ThornhillGraphics::drawText(reason, 20, 170, 2);
    }

    // For now, halt upon getting a kernel panic.
    for (;;) {}

}

extern "C" void interrupt_exception_handler(interrupt_state_t interruptState) {
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

// Not currently randomized. Just implemented for bug checking.
#define STACK_CHK_GUARD 0x5fbb6beef86cd9f4
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

extern "C" [[noreturn]] void __stack_chk_fail(void) {
    Kernel::panic("Stack check failure.");
}
