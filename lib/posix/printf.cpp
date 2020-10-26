#include "printf.hpp"

#define PRINTF_PAD_RIGHT    1
#define PRINTF_PAD_ZERO     2

// Should be enough to contain a 64-bit integer.
#define PRINTF_INT_BUF_LEN  22

namespace POSIX {

    static void appendChar(char** out, int c) {
        **out = c;
        ++(*out);
    }

    static int appendString(char** out, const char* str, int width, int pad) {

        register int printedCharacters = 0, padChar = ' ';
        if (width > 0) {
            register int length = 0;
            register const char* ptr;

            for (ptr = str; *ptr; ++ptr) ++length;
            if (length >= width) width = 0;
            else width -= length;

            if (pad & PRINTF_PAD_ZERO) padChar = '0';
        }

        if (!(pad & PRINTF_PAD_RIGHT)) {
            for (; width > 0; --width) {
                appendChar(out, padChar);
                ++printedCharacters;
            }
        }

        for (; *str; ++str) {
            appendChar(out, *str);
            ++printedCharacters;
        }

        for (; width > 0; --width) {
            appendChar(out, padChar);
            ++printedCharacters;
        }

        return printedCharacters;

    }

    static int appendInt(char** out, int i, int base, bool isSigned, int width, int pad, int baseStartLetter) {

        char buf[PRINTF_INT_BUF_LEN];
        register char *s;
        register int t, neg = 0, printedCharacters = 0;
        register unsigned int u = i;

        if (i == 0) {
            buf[0] = '0';
            buf[1] = '\0';
            return appendString(out, buf, width, pad);
        }

        if (isSigned && base == 10 && i < 0) {
            neg = 1;
            u = -i;
        }

        s = buf + PRINTF_INT_BUF_LEN - 1;
        *s = '\0';

        // Base converstion.
        while (u) {
            t = u % base;
            if (t >= 10) t += baseStartLetter - '0' - 10;
            *--s = t + '0';
            u /= base;
        }

        if (neg) {
            if (width && (pad & PRINTF_PAD_ZERO)) {
                appendChar(out, '-');
                ++printedCharacters;
                --width;
            } else {
                *--s = '-';
            }
        }

        return printedCharacters + appendString(out, s, width, pad);

    }

    static int print(char** out, int* varg) {

        register int width, pad;
        register int printedCharacters = 0;

        register char* format = (char*) (*varg++);
        
        for (; format != 0; ++format) {

            
            if (*format == '%') {
                
                ++format;
                width = pad = 0;

                switch (*format) {
                    case '\0': goto done;
                    case '%': goto appendChar;
                    case '-': {
                        ++format;
                        pad = PRINTF_PAD_RIGHT;
                        break;
                    }
                }

                while (*format == '0') {
                    ++format;
                    pad |= PRINTF_PAD_ZERO;
                }

                for (; *format >= '0' && *format <= '9'; ++format) {
                    width *= 10;
                    width += *format - '0';
                }

                register char* s;
                switch (*format) {
                    case 's':
                        s = *((char**) varg++);
                        printedCharacters += appendString(out, s ? s : "(null)", width, pad);
                        continue;
                        break;
                    
                    case 'd':
                        printedCharacters += appendInt(out, *varg++, 10, true, width, pad, 'a');
                        continue;
                        break;
                    
                    case 'u':
                        printedCharacters += appendInt(out, *varg++, 10, false, width, pad, 'a');
                        continue;
                        break;

                    case 'X':
                        printedCharacters += appendInt(out, *varg++, 16, false, width, pad, 'A');
                        continue;
                        break;
                    
                    case 'x':
                        printedCharacters += appendInt(out, *varg++, 16, false, width, pad, 'a');
                        continue;
                        break;
                    
                    case 'c':
                        char scr[2];
                        scr[0] = *varg++;
                        scr[1] = '\0';
                        printedCharacters += appendString(out, scr, width, pad);
                        break;
                        continue;
                }

            } else {
                appendChar:
                    appendChar(out, *format);
                    ++printedCharacters;
            }

        }

        done:
        return printedCharacters;

    }



    /**
     * @brief  Sends formatted output to stdout.
     * @param  format: The string containing the text that should be written to stdout.
     * @retval If successful the total number of characters written. Otherwise, a negative number.
     */
    int printf(const char* format, ...) {

        register int* varg = (int*) (&format);

        char* buf;
        int output = print(&buf, varg);

        // TODO: printf

        return output;

    }

    /**
     * @brief  Sends formatted output to a string pointed to by the str parameter.
     * @param  str: The pointer to an array of char elements where the resulting string should be stored.
     * @param  format: The string containing text to be written. Optionally including formatting tags.
     * @retval If successful the total number of characters written, excluding the null termination character. Otherwise, a negative number.
     */
    int sprintf(char* str, const char* format, ...) {
        
        register int* varg = (int*) (&format);
        return print(&str, varg);

    }

}