#include "utils.h"

int mem_compare(const void* aptr, const void* bptr, UINTN n) {
    const unsigned char *a = aptr, *b = bptr;

    for (UINTN i = 0; i < n; i++) {
        if (a[i] < b[i])
            return -1;
        else if (a[i] > b[i])
            return 1;
    }

    return 0;
}