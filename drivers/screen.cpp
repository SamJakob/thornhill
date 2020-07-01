#include <stdint.h>
#include "io.cpp"

#ifndef TH_DRIVER_SCREEN
#define TH_DRIVER_SCREEN

#define VIDEO_ADDRESS 0xb8000
#define VGA_HEIGHT 25
#define VGA_WIDTH 80
#define WHITE_ON_BLACK 0x0f
#define RED_ON_BLACK 0x0c
#define WHITE_ON_RED 0xcf

/* Screen I/O ports */
#define IO_SCREEN_CTRL 0x3d4
#define IO_SCREEN_DATA 0x3d5

// For use when memory management is implemented!
/*class Position {
    private:
        int x;
        int y;
    
    public:
        Position(int x, int y) {
            this->x = x;
            this->y = y;
        }

        int getX() {
            return this->x;
        }

        int getY() {
            return this->y;
        }
};*/

struct position_t {
    int x;
    int y;
};

class Screen {

    private:
        static position_t getCursorPosition() {
            uint16_t offset = 0;

            // Read high byte and offset.
            ThornhillIO::writeByteToPort(IO_SCREEN_CTRL, 0x0F);
            offset |= ThornhillIO::readByteFromPort(IO_SCREEN_DATA);

            // Read low byte and add to previous value.
            ThornhillIO::writeByteToPort(IO_SCREEN_CTRL, 0x0E);
            offset |= ThornhillIO::readByteFromPort(IO_SCREEN_DATA) << 8;

            position_t cursorPosition;
            cursorPosition.x = offset % VGA_WIDTH;
            cursorPosition.y = offset / VGA_WIDTH;
            return cursorPosition;
        }

        static void setCursorPosition(int x, int y) {
            uint16_t offset = y * VGA_WIDTH + x;

            // Write high byte.
            ThornhillIO::writeByteToPort(IO_SCREEN_CTRL, 0x0F);
            ThornhillIO::writeByteToPort(IO_SCREEN_DATA, (unsigned) offset & 0xFF);
            // Write low byte.
            ThornhillIO::writeByteToPort(IO_SCREEN_CTRL, 0x0E);
            ThornhillIO::writeByteToPort(IO_SCREEN_DATA, (unsigned) (offset >> 8) & 0xFF);
        }

    public:
        static void print(const char* message) {
            Screen::printAt(message, -1, -1);
        }

        static void printAt(const char* message, int x, int y) {

            // Sync up the x and y values.
            if (x >= 0 && y >= 0)
                setCursorPosition(x, y);
            else {
                position_t cursorPosition = getCursorPosition();
                x = cursorPosition.x;
                y = cursorPosition.y;
            }

            uint8_t* screen = reinterpret_cast<uint8_t*>(VIDEO_ADDRESS);

            while (*message != 0) {
                if (*message == '\n') {
                    x = 0;
                    y++;
                }

                *screen++ = *message++;
                *screen++ = WHITE_ON_BLACK;

                //setCursorPosition(x, y);
            }
        }
        
        static void clear() {
            int screenSize = VGA_WIDTH * VGA_HEIGHT;
            char* screen = reinterpret_cast<char*>(VIDEO_ADDRESS);

            for (int i = 0; i < screenSize; i++) {
                *screen++ = ' ';
                *screen++ = WHITE_ON_BLACK;
            }

            setCursorPosition(1, 0);
        }

};

#endif