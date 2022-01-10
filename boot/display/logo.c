#include "../config.h"
#include "logo.h"

#include <efilib.h>

VOID THBRenderBackground(EFI_GRAPHICS_OUTPUT_PROTOCOL* Graphics) {

    EFI_STATUS Status;

    BOOLEAN NeedsBlit = TRUE;

    UINT32* BackgroundBlitBuffer;
    Status = ST->BootServices->AllocatePool(EfiLoaderData, Graphics->Mode->FrameBufferSize,
                                            (void**)&BackgroundBlitBuffer);

    // If we failed to allocate the memory pool for a high-speed blit, we'll just fill the
    // framebuffer directly.
    if (EFI_ERROR(Status)) {
        BackgroundBlitBuffer = (UINT32*) Graphics->Mode->FrameBufferBase;
        NeedsBlit = FALSE;
    }

CheckIfNeedsManualFill:
    // Do a manual fill if NeedsBlit is false.
    for (UINTN Index = 0; Index < Graphics->Mode->FrameBufferSize / 4; Index++) {
        BackgroundBlitBuffer[Index] = BACKGROUND_COLOR;
    }
    if (!NeedsBlit) return;

    // Otherwise, simply blit the buffer.
    Status = Graphics->Blt(
        Graphics, (EFI_GRAPHICS_OUTPUT_BLT_PIXEL*)BackgroundBlitBuffer, EfiBltBufferToVideo,
        // SourceX, SourceY
        0, 0,
        // DestX, DestY
        0, 0,
        // Width, Height
        Graphics->Mode->Info->HorizontalResolution, Graphics->Mode->Info->VerticalResolution,
        // Delta
        0
    );
    if (EFI_ERROR(Status)) {
        NeedsBlit = FALSE;
        BackgroundBlitBuffer = (UINT32*) Graphics->Mode->FrameBufferBase;
        goto CheckIfNeedsManualFill;
    }

}

VOID THBRenderBootLogo(EFI_FILE* Image, EFI_GRAPHICS_OUTPUT_PROTOCOL* Graphics) {

    EFI_STATUS Status;
    BITMAP_IMAGE_FILE_HEADER BitmapHeader;

    /* Load the bitmap header from the Image file. */
    UINTN BitmapHeaderSize = sizeof(BITMAP_IMAGE_FILE_HEADER);
    Status = Image->Read(Image, &BitmapHeaderSize, &BitmapHeader);
    if (EFI_ERROR(Status)) return;

    /* Verify that the bitmap type is valid */
    // (This also allows us to verify that the file was loaded correctly.)
    if (BitmapHeader.Type != BITMAP_IMAGE_TYPE_BM) return;

    BITMAP_IMAGE_INFO_HEADER BitmapInfoHeader;

    /* Load the bitmap info header from the Image file */
    // (This header will immediately follow the previous one in the file.)
    UINTN BitmapInfoHeaderSize = sizeof(BITMAP_IMAGE_INFO_HEADER);
    Status = Image->Read(Image, &BitmapInfoHeaderSize, &BitmapInfoHeader);
    if (EFI_ERROR(Status)) return;

    /* Seek to bitmap data */
    Status = Image->SetPosition(Image, BitmapHeader.Offset);
    if (EFI_ERROR(Status)) return;

    /* Allocate a memory pool to accommodate the bitmap data */
    UINT8* LogoBitmapData;
    UINTN LogoBitmapDataSize = BitmapInfoHeader.BitmapSize;
    Status = ST->BootServices->AllocatePool(EfiLoaderData, LogoBitmapDataSize,
                                   (void**)&LogoBitmapData);
    if (EFI_ERROR(Status)) return;

    /* Read the bitmap data into the allocated memory */
    Status = Image->Read(Image, &LogoBitmapDataSize, LogoBitmapData);
    if (EFI_ERROR(Status)) {
        ST->BootServices->FreePool(LogoBitmapData);
    }

    // If the bitmap's height property is positive, the bitmap will need to be flipped to
    // display correctly.
    UINTN X, Y, POS, BLTPOS;
    if (BitmapInfoHeader.Height > 0) {
        for (Y = BitmapInfoHeader.Height; Y > ((UINTN) BitmapInfoHeader.Height / 2); Y--) {
            for (X = 0; X < BitmapInfoHeader.Width; X++) {
                POS = (Y - 1) * BitmapInfoHeader.Width + X;
                BLTPOS = (BitmapInfoHeader.Height - Y) * BitmapInfoHeader.Width + X;

                UINT32 BLTPOS_DATA = ((UINT32*)LogoBitmapData)[BLTPOS];
                ((UINT32*)LogoBitmapData)[BLTPOS] = ((UINT32*)LogoBitmapData)[POS];
                ((UINT32*)LogoBitmapData)[POS] = BLTPOS_DATA;
            }
        }
    }

    Graphics->Blt(
        Graphics, (EFI_GRAPHICS_OUTPUT_BLT_PIXEL*)LogoBitmapData, EfiBltBufferToVideo,
        // SourceX, SourceY
        0, 0,
        // DestX
        (Graphics->Mode->Info->HorizontalResolution / 2) - (BitmapInfoHeader.Width / 2),
        // DestY
        (Graphics->Mode->Info->VerticalResolution / 2) - (BitmapInfoHeader.Height / 2),
        // Width, Height
        BitmapInfoHeader.Width, BitmapInfoHeader.Height,
        // Delta
        0
    );

    ST->BootServices->FreePool(LogoBitmapData);

}
