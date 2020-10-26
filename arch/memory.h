/*
 * The expected use of this class is to be included in the relevant archiotecture's "include.cpp"
 * file.
 */

#ifndef TH_ARCH_ANY_MEM
#define TH_ARCH_ANY_MEM

#define PAGE_ALIGN(__x) ((__x) + TH_ARCH_PAGE_SIZE - ((__x) % TH_ARCH_PAGE_SIZE))

#define PAGE_ALIGN_UP(__x)              \
        ((__x % TH_ARCH_PAGE_SIZE == 0) \
            ? (__x)                     \
            : (__x) + TH_ARCH_PAGE_SIZE - ((__x) % TH_ARCH_PAGE_SIZE))

#define PAGE_ALIGN_DOWN(__x)            \
        ((__x) - ((__x) % TH_ARCH_PAGE_SIZE))

#define IS_PAGE_ALIGNED(__x)            \
        (__x % TH_ARCH_PAGE_SIZE == 0)

#endif