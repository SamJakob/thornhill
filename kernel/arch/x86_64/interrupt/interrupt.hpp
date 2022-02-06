#include <drivers/io.hpp>
#include <stdint.h>

#pragma once

extern "C" {
    #include "kernel/arch/x86_64/gdt/thornhill_gdt.h"

    #include "thornhill_idt.h"
    #include "thornhill_isr.h"
}

typedef enum {
    CPU_PRIV_LEVEL_KERNEL = 0,
    CPU_PRIV_LEVEL_HIGH = 1,
    CPU_PRIV_LEVEL_LOW = 2,
    CPU_PRIV_LEVEL_USER = 3
} CpuPrivilegeLevel;

class ThornhillInterrupt {
    private:
        static ThornhillInterruptHandler interruptHandlers[256];

        /**
         * @brief  Generates the type field for the IDT descriptor in a more readable way.
         * @note   The IDT is in the following format:
         * <code>
           0b1_00_0_1110
             ^ ^^ ^ ^
             | |  | |- bits 1110 = "32 bit interrupt gate"
             | |  |
             | |  |- Set to 0 for interrupt gates.
             | |
             | |- Privilege level of caller {0=kernel, ..., 3=user}
             |
             |- Interrupt is present.
         * </code>
         * @param  gateType: The IDT gate type. Refer to CpuIdtGateType in thornhill_idt.h for more information.
         * @param  interruptPresent: Whether or not the interrupt should be activated.
         * @param  privilegeLevel: The CPU privilege level the calling descriptor should have.
         * @param  storageSegment: Set to 0 for unused interrupts.
         * @retval The packed IDT descriptor type field in the form of an unsigned byte.
         */
        static uint8_t generateIdtDescriptorType(CpuIdtGateType gateType, CpuPrivilegeLevel privilegeLevel = CPU_PRIV_LEVEL_KERNEL, bool storageSegment = 0, bool interruptPresent = 1);

    public:
      static void setAllowInterrupts(bool allowInterrupts);

      static void setupInterrupts();

      static void setIdtGate(int gateNumber, uint64_t handler);

      static void registerInterruptHandler(uint8_t interrupt, ThornhillInterruptHandler handler);

      static bool hasHandlerFor(uint8_t interrupt);

      static ThornhillInterruptHandler getHandlerFor(uint8_t interrupt);

      static void triggerHandlerFor(uint8_t interrupt, interrupt_state_t interruptState);
};
