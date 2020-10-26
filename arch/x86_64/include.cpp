#pragma once

#include "drivers/io.hpp"

#include "arch/x86_64/interrupt/interrupt.cpp"
#include "arch/x86_64/gdt/gdt.cpp"

#ifndef TH_ARCH_PAGE_SIZE
#define TH_ARCH_PAGE_SIZE (4096)
#endif

#include "arch/memory.h"