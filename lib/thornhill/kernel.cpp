#include "kernel.hpp"

#include "drivers/hardware/serial.hpp"
#include "lib/std.hpp"
#include "lib/thornhill.hpp"

using namespace std;
using namespace Thornhill;

namespace Thornhill::Kernel {

void printChar(char c) { ThornhillSerial::writeCharacter(c); }
void print(const char* message) { ThornhillSerial::write(message); }

int vprintf(const char* fmt, va_list arg) {
    int length = 0;

    char* strPtr;
    char buffer[512];

    char c;
    while ((c = *fmt++)) {

        if ('%' == c) {
            switch ((c = *fmt++)) {
                /* %% => print a single % symbol (escape) */
                case '%':
                    printChar('%');
                    length++;
                    break;

                /* %c => print a character */
                case 'c':
                    printChar((char) va_arg(arg, int));
                    length++;
                    break;

                /* %s => print a string */
                case 's':
                    strPtr = va_arg(arg, char*);
                    print(strPtr);
                    length += strlen(strPtr);
                    break;

                case 'd':
                    itoa(buffer, va_arg(arg, int), 10, 512);
                    print(buffer);
                    length += strlen(buffer);
                    break;

                case 'x':
                    break;
                
                case 'n':
                    printChar('\r');
                    printChar('\n');
                    length += 2;
                    break;
                }
        } else {
            printChar(c);
            length++;
        }
    }

    return length;
}

void printf(const char* fmt...) {
    va_list arg;

    va_start(arg, fmt);
    vprintf(fmt, arg);
    va_end(arg);
}

}