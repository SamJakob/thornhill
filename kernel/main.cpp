extern "C" {
    #include "../boot/handoff.h"
}

#include "../drivers/graphics.cpp"
#include "descriptor/gdt.cpp"
#include "utils.cpp"

void main(ThornhillHandoff* thornhillHandoff) {

    ThornhillGraphics::initialize(thornhillHandoff->screen);

    ThornhillGraphics::drawStatusBar();
    ThornhillGraphics::drawText("Howdy!", 20, 12, 2);

    // TODO: Implement TTY.

}

extern "C" void _start(
    ThornhillHandoff* thornhillHandoff
) {
    
    ThornhillGDT::setup();
    
    main(thornhillHandoff);
    for(;;) {}

}

/*
struct interrupt_frame;
__attribute__((interrupt)) void interrupt_handler(struct interrupt_frame* frame) {
    // TODO: something.
    ThornhillGraphics::drawText("interrupt", 20, 50, 2);
}
*/