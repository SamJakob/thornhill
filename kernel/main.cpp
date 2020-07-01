#include "../drivers/screen.cpp"

void main() {

    Screen::clear();
    Screen::print("Howdy!");
    
}

extern "C" void _start() {
    
    main();

}