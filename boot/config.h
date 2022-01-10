#pragma once

/**
 * The filename of the kernel image that should be loaded by the bootloader.
 * Default is "kernel".
 * You must prefix your string with the L modifier, e.g., L"kernel" to produce the UTF-16 string
 * expected by GNU-EFI.
 */
#define KERNEL_FILENAME         L"kernel"

/**
 * The background color to display when booting.
 *
 * This is displayed before the logo is loaded.
 * This should be a hexadecimal color, in the format 0xXXXXXX, where you should replace the six
 * Xs with your 6-digit hex color.
 * For example, #545454 would be 0x545454.
 *
 * (!) THIS IS IGNORED IF SHOW_INFO_MESSAGES IS TRUE.
 */
#define BACKGROUND_COLOR        0x545454

/**
 * The filename of the logo that should be displayed by the bootloader whilst the system
 * is booting. PRESENTLY, ONLY BMP IS (TRIVIALLY) SUPPORTED.
 *
 * You must prefix your string with the L modifier, e.g., L"thornhill.bmp" to produce the UTF-16
 * string expected by GNU-EFI.
 *
 * (!) THIS IS IGNORED IF SHOW_INFO_MESSAGES IS TRUE.
 */
#define LOGO_FILENAME           L"thornhill.bmp"

/**
 * The number of seconds to delay booting the system to allow the user to select boot options.
 * The minimum is 0 (which disables the boot delay) and the maximum is 60.
 *
 * (!) THIS IS IGNORED IF SHOW_INFO_MESSAGES IS TRUE.
 */
#define BOOT_DELAY_SECONDS      5

/**
 * Whether information messages should be shown by the bootloader.
 * Default is false, which implies silent boot. You can set this to true to display logging
 * messages which can be helpful for working on the bootloader or debugging early parts of the
 * kernel.
 */
#define SHOW_INFO_MESSAGES      false
