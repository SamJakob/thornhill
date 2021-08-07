#include "utils.h"

int mem_compare(const void* aptr, const void* bptr, UINTN n) {
    const unsigned char *a = aptr, *b = bptr;

    for (UINTN i = 0; i < n; i++) {
        if (a[i] < b[i])
            return -1;
        else if (a[i] > b[i])
            return 1;
    }

    return 0;
}

EFI_STATUS THBAttemptExitBootServices(EFI_HANDLE* ImageHandle) {

    EFI_STATUS Status;

    EFI_MEMORY_DESCRIPTOR* Map = NULL;
    UINTN MapSize, MapKey;
    UINTN DescriptorSize;
    UINT32 DescriptorVersion;

    Status = ST->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
    Status = ST->BootServices->AllocatePool(EfiLoaderData, MapSize, (void**)&Map);
    Status = ST->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);

    if (EFI_ERROR(Status))
        return Status;

    Status = ST->BootServices->ExitBootServices(ImageHandle, MapKey);
    return Status;

}