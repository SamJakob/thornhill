#include "file.h"

#include <efilib.h>

EFI_FILE* THBLocateFile(EFI_HANDLE* Handle, CHAR16* FileName) {
    EFI_STATUS Status;
    EFI_HANDLE_PROTOCOL HandleProtocol = ST->BootServices->HandleProtocol;

    EFI_LOADED_IMAGE_PROTOCOL* LoadedImage;
    Status = HandleProtocol(*Handle, &gEfiLoadedImageProtocolGuid, (void**) &LoadedImage);
    if (EFI_ERROR(Status)) return NULL;

    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSystem;
    Status = HandleProtocol(LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**) &FileSystem);
    if (EFI_ERROR(Status)) return NULL;

    EFI_FILE* Root;
    FileSystem->OpenVolume(FileSystem, &Root);

    EFI_FILE* File;
    Status = Root->Open(Root, &File, FileName, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);

    if (EFI_ERROR(Status))
        return NULL;
    else
        return File;
}
