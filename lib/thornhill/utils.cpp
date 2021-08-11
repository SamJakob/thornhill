#include "utils.hpp"

#include "lib/std/string.hpp"
#include "drivers/hardware/serial.hpp"

using namespace std;

namespace Thornhill {

    void memswap(char* a, char* b) {
        char temp = *b;
        *b = *a;
        *a = temp;
    }

    const char* itoa(char* buffer, int value, int base, int bufferMax) {
        int i = 0;
        bool isNegative = false;

        // If the base is invalid (i.e., less than 2 or greater than 16),
        // we'll just return a hyphen.
        if (base < 2 || base > 16) {
            buffer[i++] = '-';
            buffer[i] = 0;
            return &buffer[0];
        }

        // If the value is zero, we'll just straight up return zero.
        if (value == 0) {
            buffer[i++] = '0';
            buffer[i] = 0;
            return &buffer[0];
        }

        // Handle negative values by setting a flag, so we can just
        // prepend the negative sign afterwards.
        // We only want to do negative values for base 10 - it doesn't
        // really make sense for any other values because they
        // represent binary.
        if (value < 0 && base == 10) {
            isNegative = true;
            value = -value;
        }

        // Now perform the conversion.
        // If bufferMax > 0 then we'll check to make sure we don't
        // exceed it.
        while (value != 0 && (bufferMax == 0 || bufferMax < i)) {
            int remainder = value % base;
            buffer[i++] = "0123456789abcdef"[remainder];
            value /= base;
        }

        // If the number was negative, we'll append the
        // negative sign.
        if (isNegative)
            buffer[i++] = '-';

        // Finally, we'll append the null terminator
        // and reverse the string.
        buffer[i] = 0;
        strrev(&buffer[0], i);

        return &buffer[0];
    }
}