#include "loader.h"

#pragma once

#define MASK_PAGE_ENTRY_PRESENT(x) ((x) & 1)
#define MASK_PAGE_ENTRY_ADDR(x) ( (((x) >> 12) & (((uint64_t) 1 << 39) - 1)) << 12 )

#define IS_VADDR_VALID(x) (((x) >> 47) == 0 || ((x) >> 47) == (1 << 17) - 1)

#define MASK_VADDR_SIGNEXT(x) ((x) >> 48)
#define MASK_VADDR_P4_ADDR(x) (((x) >> 39) & ((1 << 9) - 1))
#define MASK_VADDR_P3_ADDR(x) (((x) >> 30) & ((1 << 9) - 1))
#define MASK_VADDR_P2_ADDR(x) (((x) >> 21) & ((1 << 9) - 1))
#define MASK_VADDR_P1_ADDR(x) (((x) >> 12) & ((1 << 9) - 1))
#define MASK_VADDR_OFFSET(x)  ((x) & ((1 << 12) - 1))

/**
 * @brief Sets up paging for the OS kernel.
 */
EFI_STATUS THBSetupPaging(THBKernelSymbols* KernelSymbols);