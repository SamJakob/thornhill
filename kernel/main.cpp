extern "C" {
    #include "../boot/handoff.h"
}

#include "../drivers/graphics.cpp"
#include "memory/gdt.cpp"
#include "interrupt/interrupt.cpp"
#include "utils.cpp"

void main(ThornhillHandoff* thornhillHandoff) {

    ThornhillGraphics::initialize(thornhillHandoff->screen);
    ThornhillInterrupt::setup();

    ThornhillGraphics::drawStatusBar();
    ThornhillGraphics::drawText("Howdy!", 20, 12, 2);

    // TODO: Implement TTY.
    __asm__ __volatile__("int $1");

}

extern "C" void _start(
    ThornhillHandoff* thornhillHandoff
) {
    
    ThornhillGDT::setup();
    
    main(thornhillHandoff);
    for(;;) {}

}

typedef struct {
    uint64_t cr0, cr2, cr3, cr4, rbp, rsi, rdi, rax, rbx, rcx, rdx;
    uint64_t int_no, err_code; /* Interrupt number and error code (if applicable) */
    uint64_t eip, cs, eflags, useresp, ss; /* Pushed by the processor automatically */
} interrupt_state_t;

extern "C" void interrupt_handler(interrupt_state_t interruptState) {
    ThornhillGraphics::clear(rgb(34, 34, 34));

    ThornhillGraphics::drawText("Thornhill", 20, 50, 6);
    ThornhillGraphics::drawText("// Your computer needs to be restarted.", 20, 100, 2);
    
    ThornhillGraphics::drawText(ThornhillUtils::int_to_ascii(interruptState.int_no), 20, 150, 2);
    ThornhillGraphics::drawText(exceptionMessages[interruptState.int_no], 20, 170, 2);
}
