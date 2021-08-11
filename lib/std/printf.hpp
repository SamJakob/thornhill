#ifndef CTHORN_STD_PRINTF
#define CTHORN_STD_PRINTF

namespace std {

    int print(char** out, int* varg);

    /**
     * @brief  Sends formatted output to stdout.
     * @param  format: The string containing the text that should be written to stdout.
     * @retval If successful the total number of characters written. Otherwise, a negative number.
     */
    int printf(const char* format, ...);

    /**
     * @brief  Sends formatted output to a string pointed to by the str parameter.
     * @param  str: The pointer to an array of char elements where the resulting string should be
     * stored.
     * @param  format: The string containing text to be written. Optionally including formatting
     * tags.
     * @retval If successful the total number of characters written, excluding the null termination
     * character. Otherwise, a negative number.
     */
    int sprintf(char* str, const char* format, ...);

}

#endif