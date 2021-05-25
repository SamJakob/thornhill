#include "drivers/io.hpp"

#include "kernel/arch/x86_64/gdt/gdt.cpp"
#include "kernel/arch/x86_64/interrupt/interrupt.cpp"

#ifndef TH_ARCH_PAGE_SIZE
#define TH_ARCH_PAGE_SIZE (4096)
#endif

#include <kernel/arch/memory.h>