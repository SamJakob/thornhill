/*
 * The expected use of this class is to be included in the relevant architecture's "include.hpp"
 * file.
 */

#pragma once

#ifndef TH_ARCH
#error "The system architecture is not defined. Please include a system architecture's include.hpp."
#endif

#define PAGE_ALIGN(__x) ((__x) + TH_ARCH_PAGE_SIZE - ((__x) % TH_ARCH_PAGE_SIZE))

#define IS_PAGE_ALIGNED(__x)            \
        (__x % TH_ARCH_PAGE_SIZE == 0)

#define PAGE_ALIGN_UP(__x)              \
        (IS_PAGE_ALIGNED(__x)           \
            ? (__x)                     \
            : (__x) + TH_ARCH_PAGE_SIZE - ((__x) % TH_ARCH_PAGE_SIZE))

#define PAGE_ALIGN_DOWN(__x)            \
        ((__x) - ((__x) % TH_ARCH_PAGE_SIZE))
