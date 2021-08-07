#include "utils.hpp"

int THUtils::strlen(const char* str) {
    int len = 0;
    while (str[len] != 0) len++;
    return len;
}

const char* THUtils::int_to_ascii(int value, int base) {

    if (value == 0) return "0";

    /*
    bool wasNegative = false;
    if (value < 0) {
        value *= -1;
        wasNegative = true;
    }
    */
    
    static char buffer[32] = {0};
    int i = 30;

    for (; value && i; --i, value /= base)
        buffer[i] = "0123456789abcdef"[value % base];

    /*if (wasNegative) {
        buffer[i] = '-';
        i--;
    }*/

    return &buffer[i + 1];

}

const char* THUtils::pad_start(const char* value, char* output, char padChar, int intendedLength) {
    int valueLength = strlen(value);
    if (valueLength >= intendedLength) return value;

    // Create a new output array of intendedLength
    // (1 is added to ensure room for the appended null-termination byte.)
    int bytesToPad = intendedLength - valueLength;
    for (int i = 0; i < bytesToPad; i++) {
        output[i] = padChar;
    }

    for (int i = bytesToPad; i < intendedLength; i++) {
        output[i] = value[i - bytesToPad];
    }

    return &output[0];
}
