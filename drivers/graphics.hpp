#include "graphics.h"

#include "clock.hpp"
#include "io.hpp"
#include <stddef.h>

#include "font/font8x8/font8x8_basic.h"

#pragma once

using namespace std;

#define FONT_CHARACTER_WIDTH 8
#define FONT_CHARACTER_HEIGHT 8

#define rgb(r, g, b) {r, g, b}
uint32_t pixel(PixelFormat format, Color color);

class ThornhillGraphics {

    private:
        static Screen screen;

    public:
        //
        // Initialization
        //

        static void initialize(Screen screen);

        //
        // Getters
        //
        static Screen* getScreen();

        //
        // Text & Basic
        //
        static void drawCharacter(char character, uint16_t x, uint16_t y, int scale = 1);
        static void drawText(const char* characters, uint16_t x, uint16_t y, int scale = 1,
                             int padding = 0);
        static void drawRect(Color color, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
        static void drawTextFuncky(char* characters, size_t start, size_t len, uint16_t x, uint16_t y, int scale,
				   int padding, uint16_t max, int* endX, int* endY);


        //
        // UI
        //
        static void drawTime(ThornhillSystemTime* time);
        static void drawTTY();
        static void clear(Color color = rgb(0, 0, 0));
};
