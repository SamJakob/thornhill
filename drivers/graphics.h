#include <stdint.h>

#ifndef TH_DRIVER_GRAPHICS
#define TH_DRIVER_GRAPHICS

#define PIXEL_SIZE 32

typedef enum {
  THPixelRedGreenBlueReserved8BitPerColor,
  THPixelBlueGreenRedReserved8BitPerColor
} PixelFormat;

typedef struct {

    uint32_t width;
    uint32_t height;
    uint32_t pixels_per_scanline;
    uint64_t frame_buffer_base;
    uint64_t frame_buffer_size;
    PixelFormat pixel_format;

} Screen;

typedef struct {
    uint8_t blue;
    uint8_t red;
    uint8_t green;
} Color;

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

#endif