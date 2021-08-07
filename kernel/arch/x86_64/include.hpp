#include "drivers/io.hpp"

#include "kernel/arch/x86_64/gdt/gdt.hpp"
#include "kernel/arch/x86_64/interrupt/interrupt.hpp"

#ifndef TH_ARCH_PAGE_SIZE
#define TH_ARCH_PAGE_SIZE (4096)
#endif

#include <kernel/arch/memory.h>