extern "C" {
    #include "../boot/handoff.h"
}

#include "../drivers/graphics.cpp"
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
    
    main(thornhillHandoff);
    for(;;) {}

}