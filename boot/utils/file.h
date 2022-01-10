#include <efi.h>

#pragma once

/**
 * Attempt to locate the specified file on the boot device. Returns a null
 * pointer if the file couldn't be located or loaded.
 *
 * @param Handle The handle to the current EFI application.
 * @param FileName The name of the file to load from the root device.
 *
 * @return A pointer to the EFI file object, or null if it couldn't be loaded.
 */
EFI_FILE* THBLocateFile(EFI_HANDLE* Handle, CHAR16* FileName);
