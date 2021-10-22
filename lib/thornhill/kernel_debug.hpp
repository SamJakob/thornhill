#include "config/system.h"

#pragma once

#define TH_KERNEL_DEBUG_PREFIX "\u001b[33;1m[DEBUG] "
#define TH_KERNEL_DEBUG_ANSI_RESET "\u001b[0m"

namespace Thornhill {

    namespace Kernel {

        /**
         * @brief Prints a debug message in the serial debug console,
         * with a system timestamp.
         *
         * @param message The message to print in the serial console.
         */
        inline void debug(const char* message) {
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
        inline void debug(const char* moduleName, const char* message) {
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
        inline void debugf(const char* moduleName, const char* fmt...) {
#if TH_CONFIG_SYSTEM_DEBUG_MODE
            printf(TH_KERNEL_DEBUG_PREFIX "[%s]", moduleName);

            va_list arg;
            va_start(arg, fmt);
            vprintf(fmt, arg);
            va_end(arg);

            printf(TH_KERNEL_DEBUG_ANSI_RESET "%n");
#endif
        }

    }

}