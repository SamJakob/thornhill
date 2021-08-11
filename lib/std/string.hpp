#ifndef CTHORN_STD_STRING
#define CTHORN_STD_STRING

namespace std {

    /**
     * @brief Calculates the length of a C string, using the base address
     * provided.
     * 
     * @param str The base address of the string to check.
     * @return int The length of the string.
     */
    int strlen(const char* str);

    /**
     * @brief Pads a string such that its length is equal to intendedLength
     * by prepending padChar until that criteria is met.
     * 
     * If the string length is already larger than the output length, no
     * action will be performed and the ORIGINAL string pointer will be
     * returned.
     * 
     * The output buffer should be at least intendedLength in size!!!
     *
     * @param value The string to pad.
     * @param output The output buffer which will hold the transformed string.
     * @param padChar The character to use when padding the string.
     * @param intendedLength The intended final length of the string. (i.e. pad
     * until this length.)
     * @return The output pointer.
     */
    const char* strpad(const char* value, char* output, char padChar, int intendedLength);

    /**
     * @brief Reverses the specified string.
     * 
     * @param str The string to reverse.
     * @param length The length of the string, excluding null terminators.
     */
    void strrev(char* str, int length);

    class String {
        unsigned length;
        char* data;

        public:
            /**
             * @brief  Empty constructor.
             * @note   String length is set to 0.
             * @retval An empty instance of String.
             */
            String();

            /**
             * @brief  A single-character string constructor.
             * @note   String length is set to 1 and the character is effectively 'converted' to a string.
             * @param  c: A character literal.
             * @retval String data is equal to @a c and the string length is equal to 1.
             */
            String(char c);
    };
    
}

#endif
