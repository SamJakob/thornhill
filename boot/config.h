#pragma once

/**
 * The filename of the kernel image that should be loaded by the bootloader.
 * Default is "kernel".
 * You must prefix your string with the L modifier, e.g., L"kernel" to produce the UTF-16 string
 * expected by GNU-EFI.
 */
#define KERNEL_FILENAME         L"kernel"

/**
 * Whether information messages should be shown by the bootloader.
 * Default is false, which implies silent boot. You can set this to true to display logging
 * messages which can be helpful for working on the bootloader or debugging early parts of the
 * kernel.
 */
#define SHOW_INFO_MESSAGES      false
