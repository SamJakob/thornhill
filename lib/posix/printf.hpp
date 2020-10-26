#ifndef CTHORN_POSIX_PRINTF
#define CTHORN_POSIX_PRINTF

namespace POSIX {

    int printf(const char* format, ...);
    int sprintf(char* str, const char* format, ...);

}

#endif