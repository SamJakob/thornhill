#pragma once

#ifdef TH_ARCH
#error "The system architecture has already been defined, it cannot be defined again."
#endif
#define TH_ARCH "x86_64"

#include "drivers/io.hpp"

#include "kernel/arch/x86_64/gdt/gdt.hpp"
#include "kernel/arch/x86_64/interrupt/interrupt.hpp"

#define TH_ARCH_PAGE_SIZE (4096)

#include <kernel/arch/memory.h>
