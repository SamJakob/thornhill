#include "memory.h"

ThornhillMemoryType THBUefiToThornhillMemoryType(EFI_MEMORY_TYPE memoryType) {

    switch (memoryType) {
        case EfiLoaderCode:
        case EfiLoaderData:
        case EfiBootServicesCode:
        case EfiBootServicesData:
        case EfiConventionalMemory:
            return THAvailableMemory;

        case EfiRuntimeServicesCode:
        case EfiRuntimeServicesData:
        case EfiACPIMemoryNVS:
        case EfiPalCode:
            return THReservedMemory;

        case EfiReservedMemoryType:
            return THUnusedMemory;

        case EfiUnusableMemory:
            return THBadMemory;

        case EfiMemoryMappedIO:
        case EfiMemoryMappedIOPortSpace:
            return THIOMemory;

        case EfiACPIReclaimMemory:
            return THACPIReclaimMemory;

        default:
            return THUnusedMemory;
    }
}