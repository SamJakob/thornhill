#include "handoff.h"

void THBPrepareHandoffData(ThornhillHandoff* HandoffData, EFI_GRAPHICS_OUTPUT_PROTOCOL* Graphics) {

    /* Prepare graphics data for handoff. */
    Screen screen;
    screen.width = Graphics->Mode->Info->HorizontalResolution;
    screen.height = Graphics->Mode->Info->VerticalResolution;
    screen.pixels_per_scanline = Graphics->Mode->Info->PixelsPerScanLine;
    screen.pixel_format = Graphics->Mode->Info->PixelFormat;
    screen.frame_buffer_base = Graphics->Mode->FrameBufferBase;
    screen.frame_buffer_size = Graphics->Mode->FrameBufferSize;
    HandoffData->screen = screen;

}