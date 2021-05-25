#include <stdbool.h>
#include <stddef.h>

#include <efi.h>
#include <efilib.h>
#include <efivar.h>
#include <efidef.h>
#include <elf.h>

#include "display/logging.h"

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {

    InitializeLib(ImageHandle, SystemTable);

    EFI_STATUS Status;
    THBPrintBanner(SystemTable, true);

    Print(L"%s", L"New bootloader not yet implemented.\r\n");
    Print(L"%s", L"(for now use the bootloader in boot_old)\r\n");

    for (;;) {}

}