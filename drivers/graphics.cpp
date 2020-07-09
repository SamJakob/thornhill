#include "graphics.h"

#include "io.cpp"
#include "../kernel/utils.cpp"

#include "../font/font8x8/font8x8_basic.hpp"
#define FONT_CHARACTER_WIDTH 8
#define FONT_CHARACTER_HEIGHT 8

class ThornhillGraphics {

    private:
        static Screen screen;

    public:
        //
        // Initialization
        //

        static void initialize(Screen screen) {
            ThornhillGraphics::screen = screen;
            ThornhillGraphics::clear();
        }


        //
        // Text
        //
        
        static void drawCharacter(char character, uint16_t x, uint16_t y, int scale = 1) {
            uint32_t* videoBuffer = (uint32_t*) screen.frame_buffer_base;

            for (int screenY = y; screenY < y + (FONT_CHARACTER_HEIGHT * scale); screenY++) {
                for (int screenX = x; screenX < x + (FONT_CHARACTER_WIDTH * scale); screenX++) {
                    uint64_t screenOffset = (screenY * screen.width) + screenX;

                    uint8_t charX = screenX - x;
                    uint8_t charY = screenY - y;

                    if ((font8x8_basic[(uint8_t) character][charY / scale] >> (charX / scale)) & 0x01)
                        videoBuffer[screenOffset] = pixel(screen.pixel_format, rgb(255, 255, 255));
                }
            }
        }

        static void drawText(char* characters, uint16_t x, uint16_t y, int scale = 1, int padding = 0) {

            uint16_t deltaX = 0;
            while (*characters != 0) {
                drawCharacter(*characters, x + deltaX, y, scale);

                deltaX += ((FONT_CHARACTER_WIDTH * scale) + padding);
                *characters++;
            }

        }



        //
        // UI
        //

        static void drawStatusBar() {
            uint32_t* videoBuffer = (uint32_t*) screen.frame_buffer_base;

            // Fill top 50px with rgb(29, 29, 29).
            for (uint32_t y = 0; y < 40; y++) {
                for (uint32_t x = 0; x < screen.width; x++) {
                    uint64_t offset = (y * screen.width) + x;
                    videoBuffer[offset] = pixel(screen.pixel_format, rgb(29, 29, 29));
                }
            }
        }


        //
        // Screen manipulation
        //

        static void clear() {
            uint32_t* videoBuffer = (uint32_t*) screen.frame_buffer_base;

            for (uint32_t y = 0; y < screen.height; y++) {
                for (uint32_t x = 0; x < screen.width; x++) {
                    uint64_t offset = (y * screen.width) + x;
                    videoBuffer[offset] = pixel(screen.pixel_format, rgb(0, 0, 0));
                }
            }
        }

};


Screen ThornhillGraphics::screen;

