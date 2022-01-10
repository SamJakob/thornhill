#include <efi.h>

#include "../utils/bitmap.h"

/**
 * Renders the background color.
 * @param Graphics The pointer to the system Graphics Output Protocol.
 */
VOID THBRenderBackground(EFI_GRAPHICS_OUTPUT_PROTOCOL* Graphics);

/**
 * Renders the specified boot logo in the center of the screen.
 *
 * @param Image The boot logo to render.
 * @param Graphics The pointer to the system Graphics Output Protocol.
 */
VOID THBRenderBootLogo(EFI_FILE* Image, EFI_GRAPHICS_OUTPUT_PROTOCOL* Graphics);
