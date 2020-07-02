#include "../drivers/screen.cpp"
#include "utils.cpp"

void main() {

    Screen::initializeTTY();

    //Screen::println("ThornhillUtils::int_to_ascii(i)");

    Screen::println("Howdy!");
    
}

extern "C" void _start() {
    
    main();

}