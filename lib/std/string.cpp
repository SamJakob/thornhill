#include "string.hpp"

#include "lib/thornhill.hpp"
using namespace Thornhill;

namespace std {

    int strlen(const char* str) {
        int len = 0;
        while (str[len] != 0)
            len++;
        return len;
    }

    const char* strpad(const char* str, char* output, char padChar, int intendedLength) {
        int valueLength = strlen(str);
        if (valueLength >= intendedLength)
            return str;

        // Create a new output array of intendedLength
        // (1 is added to ensure room for the appended null-termination byte.)
        int bytesToPad = intendedLength - valueLength;
        for (int i = 0; i < bytesToPad; i++) {
            output[i] = padChar;
        }

        for (int i = bytesToPad; i < intendedLength; i++) {
            output[i] = str[i - bytesToPad];
        }

        return &output[0];
    }

    void strrev(char* str, int length) {
        int start = 0;
        int end = length - 1;

        while (start < end) {
            memswap(str + start, str + end);
            start++;
            end--;
        }
    }

}