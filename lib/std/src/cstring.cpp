#include <cstring>

#include <thornhill>

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

        Thornhill::memzero(output, intendedLength);

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

    void strrev(char* str, unsigned int length) {
        unsigned int start = 0;
        unsigned int end = length - 1;

        while (start < end) {
            Thornhill::memswap(str + start, str + end);
            start++;
            end--;
        }
    }

    char* strtok_r(char* str, const char* delimiter, char** savePointer) {

        // If the delimiter is less than one character long or a null pointer, simply return the
        // string straight away.
        if (delimiter == nullptr || strlen(delimiter) < 1) return str;

        // If the input string's pointer is a null pointer, set it equal to the savePointer.
        if (str == nullptr) str = *savePointer;

        // If the input string is, at this point, a null byte, we've reached the end of the string
        // and there are no more tokens to parse, so we'll set savePointer to that null value and
        // simply return a null pointer.
        if (*str == null) {
            *savePointer = str;
            return nullptr;
        }

        // If there are any leading delimiters, skip over them.
        str += strspn(str, delimiter);
        // ...and if we're now at a null byte the string at this point only consisted of
        // delimiters, so we'll set savePointer to that null value and simply return a null
        // pointer.
        if (*str == null) {
            *savePointer = str;
            return nullptr;
        }

        // Then, set savePointer to the end of the delimiter,
        char* end = str + strcspn(str, delimiter);
        if (*end == null) {
            *savePointer = end;
            return str;
        }

        // Finally, null-terminate the token and move savePointer past it.
        *end = null;
        *savePointer = end + 1;
        return str;

    }

    int strspn(const char* str, const char* chars) {

        int span = 0, i = 0;

        // A map representing each of the character bytes.
        bool charPresent[256] = {};

        // Loop over chars, to determine which chars are present, then reflect this
        // in the map.
        while (chars[i] != null) {
            // chars[i] gives the ASCII code of the character at this index in the
            // chars string. We can simply set this entry in charPresent to true to reflect
            // that this character can be found.
            charPresent[(unsigned char) chars[i]] = true;
            i++;
        }

        // Then, loop over the string, checking if each character in the string has a true
        // value in the charPresent map. If it does, increase span, otherwise the loop will break
        // and span will be returned.
        i = 0;
        while (str[i] != null) {
            if (charPresent[(unsigned char) str[i]]) span++;
            else break;

            i++;
        }
        return span;

    }

    int strcspn(const char* str, const char* chars) {

        int span = 0, i = 0;

        // A map representing each of the character bytes.
        bool charPresent[256] = {};

        // Loop over chars, to determine which chars are present, then reflect this
        // in the map.
        while (chars[i] != null) {
            // chars[i] gives the ASCII code of the character at this index in the
            // chars string. We can simply set this entry in charPresent to true to reflect
            // that this character can be found.
            charPresent[(unsigned char) chars[i]] = true;
            i++;
        }

        // Then, loop over the string, checking if each character in the string has a true
        // value in the charPresent map. If it does, increase span, otherwise the loop will break
        // and span will be returned.
        i = 0;
        while (str[i] != null) {
            if (!charPresent[(unsigned char) str[i]]) span++;
            else break;

            i++;
        }
        return span;

    }

    int strcmp(const char* str1, const char* str2) {

        // Loop until the end of str1 is reached.
        while (*str1 != null) {
            // If the current character in str1 does not match the current character in str2, or
            // (implicitly) the end of the second string is reached, break out of the loop early.
            if (*str1 != *str2) break;

            // Increment both pointers to compare the next character.
            str1++;
            str2++;
        }

        // At this point the characters differ, so return the ASCII difference. Each character is
        // 'casted' as unsigned to ensure the math is correct when subtracting ASCII values >= 128.
        return *(const unsigned char*)str1 - *(const unsigned char*)str2;

    }

    int strncmp(const char* str1, const char* str2, int n) {

        int index = 0;

        // Loop until the end of str1 is reached.
        while (*str1 != null && index < n - 1) {
            // If the current character in str1 does not match the current character in str2, or
            // (implicitly) the end of the second string is reached, break out of the loop early.
            if (*str1 != *str2) break;

            // Increment both pointers to compare the next character.
            str1++;
            str2++;
            index++;
        }

        // At this point the characters differ, so return the ASCII difference. Each character is
        // 'casted' as unsigned to ensure the math is correct when subtracting ASCII values >= 128.
        return *(const unsigned char*)str1 - *(const unsigned char*)str2;

    }

}
