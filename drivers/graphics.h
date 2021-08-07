#include <stdint.h>

#ifndef TH_DRIVER_GRAPHICS_H
#define TH_DRIVER_GRAPHICS_H

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
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} Color;

#define rgb(r, g, b) {r, g, b}
uint32_t pixel(PixelFormat format, Color color);

#endif