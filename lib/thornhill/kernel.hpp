#include <stdint.h>
#include <stdarg.h>

#ifndef CTHORN_THORNHILL_KERNEL
#define CTHORN_THORNHILL_KERNEL

namespace Thornhill {

  namespace Kernel {
      [[noreturn]] void panic(const char* reason, uint64_t interruptNumber = 69);


      /**
       * @brief Prints the specified character in the serial debug
       * console.
       * 
       * @param c The character to print in the serial console.
       */
      void printChar(char c);

      /**
       * @brief Prints the specified message in the serial debug
       * console.
       * 
       * @param message The message to print in the serial console.
       * @param appendNewline Whether a newline should be appended
       * after printing.
       */
      void print(const char* message, bool appendNewline = true);

      /**
       * @brief Prints the specified string format, after applying
       * any formatting rules in the string, in the serial debug
       * console.
       * 
       * @param fmt The message to print, with formatting rules.
       * @param ... The arguments, if any, for the format.
       */
      void printf(const char* fmt...);

      /**
       * @brief Just like printf, however it uses a va_list to
       * obtain the arguments instead. This method is intended
       * for API use.
       * 
       * This also returns the number of characters printed.
       * 
       * @param fmt The message to print, with formatting rules.
       * @param arg The arguments, if any, for the format.
       * @return The number of characters printed.
       */
      int vprintf(const char* fmt, va_list arg);
    }; // namespace Kernel

}; // namespace Thornhill

#endif