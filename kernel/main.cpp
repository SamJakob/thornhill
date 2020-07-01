#include "../drivers/io.cpp"
#include "../drivers/screen.cpp"

extern "C" void _start() {
    
    Screen::clear();

    Screen::print("12345678901234567890123456789012345678901234567890");
    //Screen::print("this is a string with a line break!woo!");

}