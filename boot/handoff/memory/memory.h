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

/**
 * @brief Populates the specified ThornhillHandoff structure with
 * the handoff memory map.
 *
 * @param HandoffData The location of the handoff data in memory.
 * @return EFI_STATUS Whether computing the memory map was successful,
 * as determined by the status if internal method calls.
 */
EFI_STATUS THBPrepareHandoffMemoryMap(ThornhillHandoff* HandoffData);