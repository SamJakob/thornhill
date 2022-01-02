#include "config/system.h"

#pragma once

#define TH_KERNEL_DEBUG_PREFIX "\u001b[33;1m[DEBUG] "
#define TH_KERNEL_DEBUG_ANSI_RESET "\u001b[0m"

/*
 * Stringify the specified parameter. This is useful for 'stringifying' compiler macros, for
 * example. This macro passes it to the immediately thereafter defined '_STR' macro which
 * actually performs the stringifying as both macro calls are needed to ensure any necessary
 * expansion is performed before stringifying.
 */
#define STR(x) _STR(x)
#define _STR(x) #x

/**
 * Assert that the specified condition is true for debugging purposes by causing a kernel
 * panic if the condition is false. The resulting panic will include the file that the
 * failed assertion was in.
 *
 * This is a no-op when TH_CONFIG_SYSTEM_DEBUG_MODE is false.
 */
#define assert(x) do { if (TH_CONFIG_SYSTEM_DEBUG_MODE) { \
    /* If the system is in debug mode, print the assertion failure message. */ \
    if (!(x)) Thornhill::Kernel::panic("Assertion failed: " #x, 69, __FILE__ ":" STR(__LINE__)); \
} } while (0)

namespace Thornhill {

    namespace Kernel {

        /**
         * @brief Prints a debug message in the serial debug console,
         * with a system timestamp.
         *
         * @param message The message to print in the serial console.
         */
        inline void debug([[maybe_unused]] const char* message) {
#if TH_CONFIG_SYSTEM_DEBUG_MODE
            printf(TH_KERNEL_DEBUG_PREFIX "%s" TH_KERNEL_DEBUG_ANSI_RESET "%n", message);
#endif
        }

        /**
         * Same as `debug`, however it includes a module name, which
         * allows for separation of logs by module.
         *
         * @param moduleName The name of the module generating the log
         * entry.
         * @param message The message to print in the serial console.
         * @see debug
         */
        inline void debug([[maybe_unused]] const char* moduleName, [[maybe_unused]] const char* message) {
#if TH_CONFIG_SYSTEM_DEBUG_MODE
            printf(TH_KERNEL_DEBUG_PREFIX "[%s] %s" TH_KERNEL_DEBUG_ANSI_RESET "%n", moduleName, message);
#endif
        }

        /**
         * @brief Prints a debug message in the serial debug console,
         * with a system timestamp, after applying any formatting rules
         * in the string, in the serial debug console. Additionally,
         * this includes a module name to allow separation of logs by
         * module.
         *
         * @param moduleName The name of the module generating the log
         * entry.
         * @param fmt The message to print, with formatting rules.
         * @param ... The arguments, if any, for the format.
         */
        inline void debugf([[maybe_unused]] const char* moduleName, [[maybe_unused]] const char* fmt...) {
#if TH_CONFIG_SYSTEM_DEBUG_MODE
            printf(TH_KERNEL_DEBUG_PREFIX "[%s] ", moduleName);

            va_list arg;
            va_start(arg, fmt);
            vprintf(fmt, arg);
            va_end(arg);

            printf(TH_KERNEL_DEBUG_ANSI_RESET "%n");
#endif
        }

    }

}
