#ifndef THORNHILL_UTILS
#define THORNHILL_UTILS

namespace THUtils {

    int strlen(const char* str);

    const char* int_to_ascii(int value, int base = 10);

    /**
     * @brief  Pads a string such that its length is equal to intendedLength
     *         by prepending padChar until that criteria is met.
     * @note
     * @param  value: The string to pad.
     * @param  padChar: The character to use when padding the string.
     * @param  intendedLength: The intended length of the padded string (i.e. pad until the string is
     * this length)
     * @retval The string with padding.
     */
    const char* pad_start(const char* value, char* output, char padChar, int intendedLength);

}

#endif