#include <stdint.h>
#include "io.cpp"

#include "vga.h"
#include "../kernel/utils.cpp"

#ifndef TH_DRIVER_SCREEN
#define TH_DRIVER_SCREEN

#define VIDEO_ADDRESS 0xb8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

/* Screen I/O ports */
#define IO_SCREEN_CTRL 0x3d4
#define IO_SCREEN_DATA 0x3d5

struct position_t {
    int x;
    int y;
};

class Screen {

    private:
        static position_t cursor_position;
        static uint8_t text_color;
        static uint16_t* terminal_buffer;

        static uint16_t computeOffset(int x, int y) {
            return (y * VGA_WIDTH) + x;
        }

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
            uint16_t offset = computeOffset(x, y);

            // Write high byte.
            ThornhillIO::writeByteToPort(IO_SCREEN_CTRL, 0x0F);
            ThornhillIO::writeByteToPort(IO_SCREEN_DATA, (uint8_t) (offset & 0xFF));
            // Write low byte.
            ThornhillIO::writeByteToPort(IO_SCREEN_CTRL, 0x0E);
            ThornhillIO::writeByteToPort(IO_SCREEN_DATA, (uint8_t) ((offset >> 8) & 0xFF));
        }

    public:
        //
        // Initialization
        //

        static void initializeTTY() {
            cursor_position.x = 0;
            cursor_position.y = 0;
            text_color = vga_character_palette(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

            Screen::clear();
        }


        //
        // Printing text
        //

        static void printCharacterAt(unsigned char character, uint8_t color, int x, int y) {
            uint16_t* screen = reinterpret_cast<uint16_t*>(VIDEO_ADDRESS);

            uint16_t offset = computeOffset(x, y);
            screen[offset] = vga_character(character, color);
            setCursorPosition(x + 1, y);
        }

        static void printCharacter(char character) {
            unsigned char unsignedCharacter = character;

            if (unsignedCharacter == '\n') {
                cursor_position.x = 0;
                cursor_position.y++;
                
                if (cursor_position.y >= VGA_HEIGHT) scrollScreen();
                setCursorPosition(0, cursor_position.y);
                return;
            }

            Screen::printCharacterAt(character, text_color, cursor_position.x, cursor_position.y);
            if (++cursor_position.x == VGA_WIDTH) {
                cursor_position.x = 0;
                if (++cursor_position.y == VGA_HEIGHT) {
                    cursor_position.y = 0;
                }
            }
        }

        static void print(const char* data) {
            for (int i = 0; i < ThornhillUtils::strlen(data); i++) Screen::printCharacter(data[i]);
        }

        static void println(const char* data) {
            print(data);
            printCharacter('\n');
        }
        

        //
        // Screen manipulation
        //

        static void clear() {
            int screenSize = VGA_WIDTH * VGA_HEIGHT;
            char* screen = reinterpret_cast<char*>(VIDEO_ADDRESS);

            for (int i = 0; i < screenSize; i++) {
                *screen++ = ' ';
                *screen++ = text_color;
            }

            setCursorPosition(0, 0);
        }

        static void scrollScreen() {
            char* screen = reinterpret_cast<char*>(VIDEO_ADDRESS);

            for (int y = 0; y < VGA_HEIGHT + 1; y++) {
                for (int x = 0; x < VGA_WIDTH; x++) {
                    uint16_t offset = computeOffset(x, y);
                    screen[offset] = screen[offset + VGA_WIDTH];
                }
            }

            cursor_position.y--;
        }

};


position_t Screen::cursor_position;
uint8_t Screen::text_color;
uint16_t* Screen::terminal_buffer;

#endif