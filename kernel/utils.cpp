#ifndef THORNHILL_UTILS
#define THORNHILL_UTILS

namespace ThornhillUtils {

    int strlen(const char* str) {
        int len = 0;
        while (str[len] != 0) len++;
        return (len);
    }

    char* int_to_ascii(int value, int base = 10) {

        if (value == 0) return "0";
        
        static char buffer[32] = {0};
        int i = 30;

        for (; value && i; --i, value /= base)
            buffer[i] = "0123456789abcdef"[value % base];

        return &buffer[i + 1];

    }

};

#endif