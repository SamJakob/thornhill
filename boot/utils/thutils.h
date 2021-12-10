#include <efi.h>
#include <efidef.h>
#include <efilib.h>

#pragma once

typedef struct {
    EFI_MEMORY_DESCRIPTOR* Map;
    UINTN MapSize, MapKey;
    UINTN DescriptorSize;
    UINT32 DescriptorVersion;
} PreBootMemoryMap;

