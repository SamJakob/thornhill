#ifndef CTHORN_STRING
#define CTHORN_STRING

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

#endif
