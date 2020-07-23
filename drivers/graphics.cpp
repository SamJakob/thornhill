#include "graphics.h"

#include "clock.hpp"
#include "io.cpp"
#include "../kernel/utils.cpp"

#ifndef TH_DRIVER_GRAPHICS
#define TH_DRIVER_GRAPHICS

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
        // Getters
        //

        static Screen* getScreen() {
            return &screen;
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

        static void drawText(const char* characters, uint16_t x, uint16_t y, int scale = 1, int padding = 0) {

            size_t pointer = 0;

            uint16_t deltaX = 0;
            while (characters[pointer] != 0) {
                drawCharacter(characters[pointer], x + deltaX, y, scale);

                deltaX += ((FONT_CHARACTER_WIDTH * scale) + padding);
                pointer++;
            }

        }

        static void drawRect(Color color, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
            uint32_t* videoBuffer = (uint32_t*) screen.frame_buffer_base;

            for (uint32_t currentY = y; currentY < height; currentY++) {
                for (uint32_t currentX = x; currentX < width; currentX++) {
                    uint64_t offset = (currentY * screen.width) + currentX;
                    videoBuffer[offset] = pixel(screen.pixel_format, color);
                }
            }
        }



        //
        // UI
        //

        static void drawStatusBar(ThornhillSystemTime* time) {
            drawRect(rgb(29, 29, 29), 0, 0, screen.width, 40);
            drawText("Thornhill", 20, 12, 2);

            //drawText(ThornhillUtils::int_to_ascii(time->month), screen.width - 360, 12, 2);
            //drawCharacter('/', screen.width - 292, 12, 2);
            //drawText(ThornhillUtils::int_to_ascii(time->day), screen.width - 320, 12, 2);
            //drawCharacter('/', screen.width - 252, 12, 2);
            //drawText(ThornhillUtils::int_to_ascii(time->fullYear), screen.width - 240, 12, 2);

            drawText(ThornhillUtils::int_to_ascii(time->hours), screen.width - 180, 12, 2);
            if(time->seconds % 2 != 0) drawCharacter(':', screen.width - 152, 12, 2);
            drawText(ThornhillUtils::int_to_ascii(time->minutes), screen.width - 140, 12, 2);
            if(time->seconds % 2 != 0) drawCharacter(':', screen.width - 112, 12, 2);
            drawText(ThornhillUtils::int_to_ascii(time->seconds), screen.width - 100, 12, 2);
            drawText(time->isPM ? "PM" : "AM", screen.width - 60, 12, 2);
        }


        //
        // Screen manipulation
        //

        static void clear(Color color = rgb(0, 0, 0)) {
            drawRect(color, 0, 0, screen.width, screen.height);
        }

};


Screen ThornhillGraphics::screen;

#endif