#include "memory.h"

ThornhillMemoryType THBUefiToThornhillMemoryType(EFI_MEMORY_TYPE memoryType) {

    switch (memoryType) {
        // Conventional memory can be passed directly as available memory.
        case EfiConventionalMemory:
            return THAvailableMemory;

        // Memory that has been used by the loader might contain sensitive information,
        // or it might contain structures that the kernel needs to replace such as the
        // boot page table. Therefore, these are reserved for kernel-only memory yet may
        // be moved to the available memory pool by the kernel.
        case EfiLoaderCode:
        case EfiLoaderData:
        case EfiBootServicesCode:
        case EfiBootServicesData:
            return THKernelMemory;

        // Memory that is reserved for services provided by the firmware is automatically
        // reserved, so it's not used by the kernel.
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