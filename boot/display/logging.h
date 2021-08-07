#include <stdbool.h>

#include <efi.h>
#include <efidef.h>
#include <efilib.h>

#ifndef TH_BOOT_DISPLAY_LOGGING_H
#define TH_BOOT_DISPLAY_LOGGING_H

/**
 * @brief Prints the system banner message. This is also used by the bootloader
 * to confirm that logging works and the system table was loaded correctly.
 * 
 * @param ShouldClearScreen Whether or not the screen should be printed before
 * displaying the banner.
 * @return EFI_STATUS The status as it was passed through from the call to
 * `SystemTable->ConOut->OutputString`.
 */
EFI_STATUS THBPrintBanner(bool ShouldClearScreen);

/**
 * @brief Prints a system message with time and date as a prefix. This simply
 * redirects the output to `SystemTable->ConOut->OutputString`. Any hard-coded
 * character arrays should be prefixed with `L` to indicate that they are UTF-16
 * strings. For example:
 * ```c
 * THPrintMessage(SystemTable, L"Example message.");
 * ```
 *
 * @param Message The UTF-16 char array to print to output.
 * @return EFI_STATUS The status as it was passed through from internal calls.
 */
EFI_STATUS THBPrintMessage(CHAR16* Message);

/**
 * @brief Prints the specified error message with extra formatting. Optionally,
 * an EFI_STATUS may be provided to print the human-readable version of the
 * status code.
 *
 * THIS IS INTENDED FOR MORE OR LESS FATAL ERRORS, that require the boot
 * process to halt - such as failing to locate the kernel, etc.
 *
 * @param ErrorMessage The human-readable error message to print.
 * @param StatusCode The EFI_STATUS to print the human-readable version of, if
 * desired, null if not desired.
 * @return EFI_STATUS The status as it was passed through from internal calls.
 */
EFI_STATUS THBErrorMessage(CHAR16* ErrorMessage, EFI_STATUS* StatusCode);

#endif