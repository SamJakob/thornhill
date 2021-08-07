#include "graphics.h"

#include "clock.hpp"
#include "io.hpp"
#include "kernel/utils.cpp"

#ifndef TH_DRIVER_GRAPHICS
#define TH_DRIVER_GRAPHICS

#include "font/font8x8/font8x8_basic.hpp"

#define FONT_CHARACTER_WIDTH 8
#define FONT_CHARACTER_HEIGHT 8

Color rgb(uint8_t r, uint8_t g, uint8_t b) {
    Color color;
    color.red = r;
    color.green = g;
    color.blue = b;
    return color;
};

uint32_t pixel(PixelFormat format, Color color) {

    switch (format) {
    case THPixelRedGreenBlueReserved8BitPerColor:
        return (color.blue << 16) | (color.green << 8) | (color.red);
        break;
    case THPixelBlueGreenRedReserved8BitPerColor:
        return (color.red << 16) | (color.green << 8) | (color.blue);
        break;
    default:
        return 0;
    }
}

class ThornhillGraphics {

  private:
    static Screen screen;

  public:
    //
    // Initialization
    //

    static void initialize(Screen screen) {
        ThornhillGraphics::screen = screen;
        ThornhillGraphics::clear(rgb(0, 128, 128));
    }

    //
    // Getters
    //

    static Screen* getScreen() { return &screen; }

    //
    // Text
    //

    static void drawCharacter(char character, uint16_t x, uint16_t y, int scale = 1) {
        uint32_t* videoBuffer = (uint32_t*)screen.frame_buffer_base;

        for (int screenY = y; screenY < y + (FONT_CHARACTER_HEIGHT * scale); screenY++) {
            for (int screenX = x; screenX < x + (FONT_CHARACTER_WIDTH * scale); screenX++) {
                uint64_t screenOffset = (screenY * screen.width) + screenX;

                uint8_t charX = screenX - x;
                uint8_t charY = screenY - y;

                if ((font8x8_basic[(uint8_t)character][charY / scale] >> (charX / scale)) & 0x01)
                    videoBuffer[screenOffset] = pixel(screen.pixel_format, rgb(255, 255, 255));
            }
        }
    }

    static void drawText(const char* characters, uint16_t x, uint16_t y, int scale = 1,
                         int padding = 0) {

        size_t pointer = 0;

        uint16_t deltaX = 0;
        while (characters[pointer] != 0) {
            drawCharacter(characters[pointer], x + deltaX, y, scale);

            deltaX += ((FONT_CHARACTER_WIDTH * scale) + padding);
            pointer++;
        }
    }

    static void drawRect(Color color, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
        uint32_t* videoBuffer = (uint32_t*)screen.frame_buffer_base;

        for (uint32_t currentY = y; currentY < y + height; currentY++) {
            for (uint32_t currentX = x; currentX < x + width; currentX++) {
                uint64_t offset = (currentY * screen.width) + currentX;
                videoBuffer[offset] = pixel(screen.pixel_format, color);
            }
        }
    }

    //
    // UI
    //

    static void drawTime(ThornhillSystemTime* time) {
        int offsetStart = 60;
        bool blinkSeparators = false;

        drawRect(rgb(0, 0, 0), offsetStart - 5, screen.height - 129, 168, 50);

        const char* month = MONTHS[time->month - 1];
        char shortMonth[4] = {NULL, NULL, NULL, NULL};

        for (int i = 0; i < 3; i++) {
            shortMonth[i] = month[i];
        }

        int intendedLength = 2;
        char output[3];

        drawText(shortMonth, offsetStart, screen.height - 100, 2);
        drawText(THUtils::pad_start(THUtils::int_to_ascii(time->day), output, '0', intendedLength),
                 offsetStart + 56, screen.height - 100, 2);
        drawText(THUtils::int_to_ascii(time->fullYear), offsetStart + 96, screen.height - 100, 2);

        drawText(
            THUtils::pad_start(THUtils::int_to_ascii(time->hours), output, '0', intendedLength),
            offsetStart, screen.height - 124, 2);
        if (!blinkSeparators || time->seconds % 2 != 0)
            drawCharacter(':', offsetStart + 28, screen.height - 124, 2);
        drawText(
            THUtils::pad_start(THUtils::int_to_ascii(time->minutes), output, '0', intendedLength),
            offsetStart + 40, screen.height - 124, 2);
        if (!blinkSeparators || time->seconds % 2 != 0)
            drawCharacter(':', offsetStart + 68, screen.height - 124, 2);
        drawText(
            THUtils::pad_start(THUtils::int_to_ascii(time->seconds), output, '0', intendedLength),
            offsetStart + 80, screen.height - 124, 2);
        drawText(time->isPM ? "PM" : "AM", offsetStart + 120, screen.height - 124, 2);
    }

    static void drawTTY() {

        int termWidth = screen.width - 240;
        int termHeight = screen.height - 240;

        // window border
        drawRect(rgb(30, 30, 30), 122, 122, termWidth + 4, termHeight + 24);
        drawRect(rgb(255, 255, 255), 119, 119, termWidth + 2, termHeight + 22);

        // title bar
        drawRect(rgb(192, 192, 192), 120, 120, termWidth, 20);

        // title bar text
        drawText("System Window", 140, 127);

        // window content
        drawRect(rgb(0, 0, 0), 120, 140, termWidth, termHeight);
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