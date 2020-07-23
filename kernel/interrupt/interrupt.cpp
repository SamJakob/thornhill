#include <stdbool.h>
#include <stdint.h>

#ifndef TH_KERN_IDT
#define TH_KERN_IDT

extern "C" {
    #include "../memory/thornhill_gdt.h"

    #include "thornhill_idt.h"
    #include "thornhill_isr.h"
}

class ThornhillInterrupt {
    private:
        static interrupt_handler_t interruptHandlers[256];

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
        static uint8_t generateIdtDescriptorType(CpuIdtGateType gateType, CpuPrivilegeLevel privilegeLevel = CPU_PRIV_LEVEL_KERNEL, bool storageSegment = 0, bool interruptPresent = 1) {
            return (interruptPresent << 7) | (privilegeLevel << 5) | (storageSegment << 4) | gateType;
        };

    public:
        static void setAllowInterrupts(bool allowInterrupts) {
            if (allowInterrupts) __asm__ __volatile__("sti");
            else __asm__ __volatile__("cli");
        }

        static void setupInterrupts() {
            // Setup IDT gates.
            setIdtGate(0, (uint64_t) isr0);
            setIdtGate(1, (uint64_t) isr1);
            setIdtGate(2, (uint64_t) isr2);
            setIdtGate(3, (uint64_t) isr3);
            setIdtGate(4, (uint64_t) isr4);
            setIdtGate(5, (uint64_t) isr5);
            setIdtGate(6, (uint64_t) isr6);
            setIdtGate(7, (uint64_t) isr7);
            setIdtGate(8, (uint64_t) isr8);
            setIdtGate(9, (uint64_t) isr9);
            setIdtGate(10, (uint64_t) isr10);
            setIdtGate(11, (uint64_t) isr11);
            setIdtGate(12, (uint64_t) isr12);
            setIdtGate(13, (uint64_t) isr13);
            setIdtGate(14, (uint64_t) isr14);
            setIdtGate(15, (uint64_t) isr15);
            setIdtGate(16, (uint64_t) isr16);
            setIdtGate(17, (uint64_t) isr17);
            setIdtGate(18, (uint64_t) isr18);
            setIdtGate(19, (uint64_t) isr19);
            setIdtGate(20, (uint64_t) isr20);
            setIdtGate(21, (uint64_t) isr21);
            setIdtGate(22, (uint64_t) isr22);
            setIdtGate(23, (uint64_t) isr23);
            setIdtGate(24, (uint64_t) isr24);
            setIdtGate(25, (uint64_t) isr25);
            setIdtGate(26, (uint64_t) isr26);
            setIdtGate(27, (uint64_t) isr27);
            setIdtGate(28, (uint64_t) isr28);
            setIdtGate(29, (uint64_t) isr29);
            setIdtGate(30, (uint64_t) isr30);
            setIdtGate(31, (uint64_t) isr31);

            // Remap the PIC.
            ThornhillIO::writeByteToPort(0x20, 0x11);
            ThornhillIO::writeByteToPort(0xA0, 0x11);
            ThornhillIO::writeByteToPort(0x21, 0x20);
            ThornhillIO::writeByteToPort(0xA1, 0x28);
            ThornhillIO::writeByteToPort(0x21, 0x04);
            ThornhillIO::writeByteToPort(0xA1, 0x02);
            ThornhillIO::writeByteToPort(0x21, 0x01);
            ThornhillIO::writeByteToPort(0xA1, 0x01);
            ThornhillIO::writeByteToPort(0x21, 0x00);
            ThornhillIO::writeByteToPort(0xA1, 0x00);

            // Install the IRQs.
            setIdtGate(32, (uint64_t) irq0);
            setIdtGate(33, (uint64_t) irq1);
            setIdtGate(34, (uint64_t) irq2);
            setIdtGate(35, (uint64_t) irq3);
            setIdtGate(36, (uint64_t) irq4);
            setIdtGate(37, (uint64_t) irq5);
            setIdtGate(38, (uint64_t) irq6);
            setIdtGate(39, (uint64_t) irq7);
            setIdtGate(40, (uint64_t) irq8);
            setIdtGate(41, (uint64_t) irq9);
            setIdtGate(42, (uint64_t) irq10);
            setIdtGate(43, (uint64_t) irq11);
            setIdtGate(44, (uint64_t) irq12);
            setIdtGate(45, (uint64_t) irq13);
            setIdtGate(46, (uint64_t) irq14);
            setIdtGate(47, (uint64_t) irq15);

            // Setup IDT.
            idt_register.base = (uint64_t) &idt;
            idt_register.limit = IDT_ENTRIES * sizeof(idt_descriptor_t) - 1;

            __asm__ __volatile__("lidt (%0)" : : "r" (&idt_register));
        }

        static void setIdtGate(int gateNumber, uint64_t handler) {
            idt[gateNumber].offset15_0 = offset15_0(handler);
            idt[gateNumber].selector = TH_KERNEL_CODE_SEGMENT;
            idt[gateNumber].null = 0;
            idt[gateNumber].type = generateIdtDescriptorType(IDT_GATE_32BIT_INT);
            idt[gateNumber].offset31_16 = offset31_16(handler);
            idt[gateNumber].offset63_32 = offset63_32(handler);
            idt[gateNumber].null2 = 0;
        }

        static void registerInterruptHandler(uint8_t interrupt, interrupt_handler_t handler) {
            interruptHandlers[interrupt] = handler;
        }

        static bool hasHandlerFor(uint8_t interrupt) {
            return interruptHandlers[interrupt] != 0;
        }

        static interrupt_handler_t getHandlerFor(uint8_t interrupt) {
            return interruptHandlers[interrupt];
        }
};

interrupt_handler_t ThornhillInterrupt::interruptHandlers[256];

#endif