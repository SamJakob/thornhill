#include <efi.h>
#include <efidef.h>
#include <efilib.h>

#include "../handoff_shared.h"
#include "memory_shared.h"

/**
 * @brief Converts an EFI_MEMORY_TYPE to a ThornhillMemoryType for the handoff
 * memory map.
 * 
 * @param memoryType The EFI_MEMORY_TYPE to convert.
 * @return ThornhillMemoryType The resulting ThornhillMemoryType.
 */
ThornhillMemoryType THBUefiToThornhillMemoryType(EFI_MEMORY_TYPE memoryType);