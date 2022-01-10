/*
 * Adapted from the following StackOverflow answer:
 * https://stackoverflow.com/a/14279511/2872279
 *
 * Using useful information from:
 * https://en.wikipedia.org/wiki/BMP_file_format
 */

#pragma once

#define BITMAP_IMAGE_TYPE_BM 0x4D42

/*
 * The bitmap file format contains:
 *
 * - bitmap file header
 * - bitmap info header
 * - palette data
 * - bitmap data
 */

#pragma pack(1)

typedef struct {
    /**
     * Bitmap file type.
     * Essentially the only used type is 0x4D42 – ASCII values representing BM.
     * This is the type used in Microsoft Windows.
     */
    UINT16 Type;

    /**
     * BitmapSize of bitmap in bytes.
     */
    UINT32 Size;

    /*! Reserved (must be 0). */
    UINT16 Reserved1;
    /*! Reserved (must be 0). */
    UINT16 Reserved2;

    /**
     * The offset, in bytes, of the bitmap data from the header.
     */
    UINT32 Offset;
} BITMAP_IMAGE_FILE_HEADER;

typedef struct {
    /**
     * Specifies the number of bytes required by the
     * 'info header' struct.
     */
    UINT32 Size;

    /**
     * Width of the image, in pixels.
     */
    UINT32 Width;

    /**
     * Height of the image, in pixels.
     * This is a signed value because per MSDN, a bitmap's height value may be set to a negative
     * value to indicate that the bitmap is top-down from upper-left instead of bottom-up from
     * lower-right.
     */
    INT32 Height;

    /**
     * The number of color planes.
     * Must be 1.
     */
    UINT16 ColorPlanes;

    /**
     * The number of bits per pixel.
     */
    UINT16 BitCount;

    /**
     * The compression method being used.
     * 0 = BI_RGB = No compression.
     */
    UINT32 CompressionMethod;

    /**
     * BitmapSize of the image, in bytes.
     */
    UINT32 BitmapSize;

    /**
     * Number of pixels per meter in the x-axis.
     */
    UINT32 XPixelsPerMeter;

    /**
     * Number of pixels per meter in the y-axis.
     */
    UINT32 YPixelsPerMeter;

    /**
     * Number of colors used by the bitmap.
     */
    UINT32 ColorsUsed;

    /**
     * Number of important colors used by the bitmap.
     */
    UINT32 ColorsImportant;
} BITMAP_IMAGE_INFO_HEADER;

#pragma pack ()
