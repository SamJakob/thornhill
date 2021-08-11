#include <stdarg.h>
#include <stdint.h>

#ifndef CTHORN_THORNHILL_UTILS
#define CTHORN_THORNHILL_UTILS

namespace Thornhill {

    /**
     * @brief Swaps the value at pointer a with the value
     * at pointer b.
     * 
     * @param a A byte pointer to swap with b.
     * @param b A byte pointer to swap with a.
     */
    void memswap(char* a, char* b);

    /**
     * @brief Converts the specified integer value to an ASCII
     * representation in the specified base. (itoa = Integer TO Ascii)
     * 
     * If the base is not specified, the default base is 10.
     * 
     * If bufferMax is specified, the number of resulting digits
     * is bufferMax - 1 (as the null byte is inserted at the end.)
     * If bufferMax is 0, it's ignored.
     * 
     * @param value The integer to convert to ASCII form.
     * @param base The numerical base of the output number.
     * @param bufferMax An optional limit on the amount of bytes that
     * will be filled into the buffer that may be specified
     * as a precautionary measure.
     * @return A pointer to the output buffer.
     */
    const char* itoa(char* buffer, int value, int base, int bufferMax = 0);

}     // namespace Thornhill

#endif