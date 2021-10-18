#include <elf.h>

#include <efi.h>
#include <efidef.h>
#include <efilib.h>

#pragma once

/**
 * @brief Holds the relevant kernel symbols, as extracted by the kernel
 * binary loader.
 */
typedef struct {
    /**
     * @brief The virtual memory base address for the kernel.
     */
    UINTN KernelBaseAddress;
    /**
     * @brief The start address for the kernel.
     */
    UINTN KernelStartAddress;
    /**
     * @brief The start address for the code segment of the kernel.
     */
    UINTN KernelCodeStartAddress;
    /**
     * @brief The end address for the code segment of the kernel.
     */
    UINTN KernelCodeEndAddress;
    /**
     * @brief The start address for the data segment of the kernel.
     */
    UINTN KernelDataStartAddress;
    /**
     * @brief The end address for the data segment of the kernel.
     */
    UINTN KernelDataEndAddress;
    /**
     * @brief The end address for the kernel.
     */
    UINTN KernelEndAddress;
} THBKernelSymbols;

/**
 * @brief Loads the specified kernel file from the same device the UEFI
 * bootloader was loaded from.
 * 
 * @param Handle The current EFI handle.
 * @param FileName The name of the kernel to load.
 * @return EFI_FILE* The EFI file object or null if it couldn't be loaded.
 */
EFI_FILE* THBLoadKernelFile(EFI_HANDLE* Handle, CHAR16* FileName);

/**
 * @brief Loads the kernel headers from the specified file and performs
 * basic checks on the headers to help ensure the kernel is valid and
 * compatible with the current system.
 * 
 * If the kernel is valid, this method returns null, otherwise it
 * returns a string indicating why the kernel is invalid.
 * 
 * Additionally, if the kernel is valid, the headers will be loaded
 * into the pointer specified by `KernelHeader`.
 * 
 * @param Kernel The kernel file to inspect.
 * @param KernelHeader A pointer to a KernelHeader struct.
 * @return CHAR16* Either null (kernel is valid) or a UTF-16 string
 * (kernel is not valid), in which case the string is the reason.
 */
CHAR16* THBCheckKernel(EFI_FILE* Kernel, Elf64_Ehdr* KernelHeader);

/**
 * @brief Loads the specified kernel file into memory, ready for it to
 * be executed once the handoff data has been prepared.
 * 
 * The `KernelHeader` parameter should be the pointer passed from
 * running THBCheckKernel as that method also loads the kernel headers
 * before they're checked.
 *
 * @param Kernel The kernel file to load.
 * @param KernelHeader A pointer to the same KernelHeader struct passed into
 * THBCheckKernel.
 * @return EFI_STATUS The status as it was passed through from internal calls.
 */
EFI_STATUS THBLoadKernel(EFI_FILE* Kernel, Elf64_Ehdr* KernelHeader, THBKernelSymbols* KernelSymbols);