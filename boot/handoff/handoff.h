#include <stddef.h>
#include <stdint.h>

#include <efi.h>
#include <efilib.h>

#include "handoff_shared.h"

/**
 * @brief Prepares and collects data and pointers in the bootloader for passing
 * to the OS kernel.
 * 
 * @param HandoffData A pointer to the handoff structure in memory.
 * @param Graphics A pointer to the current Graphics Output Protocol structure.
 */
void THBPrepareHandoffData(
    ThornhillHandoff* HandoffData,
    EFI_GRAPHICS_OUTPUT_PROTOCOL* Graphics
);