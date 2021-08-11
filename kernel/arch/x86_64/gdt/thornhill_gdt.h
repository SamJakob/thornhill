#include <stdint.h>

#ifndef TH_KERNEL_ARCH_GDT_H
#define TH_KERNEL_ARCH_GDT_H

#define TH_KERNEL_CODE_SEGMENT 0x08
#define TH_KERNEL_DATA_SEGMENT 0x10



/* GDT Descriptor Flags */

    /** The offset to be applied to check a type descriptor value. */
    #define GDTD_CHECK_TYPE_OFFSET          0x40
    
    /**
     * @brief Set by the processor if the segment is accessed. It may only be
     * cleared by software and setting this bit in software prevents GDT writes
     * upon first use.
     * 
     * If checking, offset with GDTD_CHECK_TYPE_OFFSET.
     */
    #define GDTD_TYPE_ACCESSED              0x01
    /**
     * @brief For 32-bit data segments, this sets the segment as writeable.
     * For 32-bit code segments, this sets the segment as READABLE.
     * In 64-bit mode, this is ignored for all segments.
     */
    #define GDTD_TYPE_READ_WRITE            0x02
    /**
     * @brief For code segments, this sets the segment as "conforming", which
     * "influences the privilege checks that occur on control transfers."
     * 
     * For 32-bit data segments, this sets the segment as "expand down". In
     * 64-bit mode, this is ignored for data segments.
     */
    #define GDTD_TYPE_CONFORMING            0x04
    /**
     * @brief This flag must be set for code segments and unset for data
     * segments.
     */
    #define GDTD_TYPE_EXECUTABLE            0x08

    // -------------------------------------------------------------------------

    /**
     * @brief Whether the segment is a system or user segment.
     * (0 = system, 1 = user)
     */
    #define GDTD_IS_USER(x)        ((x)        << 0x04)
    
    /**
     * @brief The CPU privilege level for the segment from 0 - 3.
     * 
     * Ignored in 64-bit mode.
     */
    #define GDTD_PRIV_LEVEL(x)     ((x & 0x03) << 0x05)
    
    /**
     * @brief Must be set for any usable segment. Causes a segment not present
     * exception if not set.
     */
    #define GDTD_PRESENT(x)        ((x)        << 0x07)
    
    /**
     * @brief Is the segment available for use by the Operating System.
     * Unused? Typically forced to 0?
     */
    #define GDTD_AVAILABLE(x)      ((x)        << 0x0C)

    /**
     * @brief Must be set for 64-bit segments, unset otherwise.
     */
    #define GDTD_LONG_MODE(x)      ((x)        << 0x0D)

    /**
     * @brief Use 32-bit instead of 16-bit operands. If GDTD_LONG_MODE is set,
     * this must be unset. In 64-bit mode this is ignored for data segments.
     */
    #define GDTD_OPERAND_SIZE(x)   ((x)        << 0x0E)

    /**
     * @brief Sets the size granuality
     * (0 = 1B - 1MB, 1 = 4KB - 4GB)
     *
     * Ignored in 64-bit mode.
     */
    #define GDTD_GRANULARITY(x)    ((x)        << 0x0F)


/* Thornhill GDT Descriptor Presets */

    #define TH_GDTD_CODE    GDTD_IS_USER(1) | GDTD_PRESENT(1) | \
                            GDTD_AVAILABLE(0) | GDTD_LONG_MODE(1) | \
                            GDTD_OPERAND_SIZE(0) | \
                            GDTD_TYPE_EXECUTABLE | GDTD_TYPE_READ_WRITE
    
    #define TH_GDTD_DATA    GDTD_IS_USER(1) | GDTD_PRESENT(1) | \
                            GDTD_AVAILABLE(0) | GDTD_LONG_MODE(1) | \
                            GDTD_TYPE_READ_WRITE
    
    #define TH_GDTD_TSS     GDTD_PRESENT(1) | GDTD_LONG_MODE(1) | \
                            GDTD_TYPE_EXECUTABLE | GDTD_TYPE_ACCESSED


/* GDT Structures */

    /**
     * @brief Computes a GDT Descriptor struct from human-readable entries.
     */
    #define gdtd(base, limit, flag) {                                         			\
        .limit15_0                  = (((uint32_t)limit)  & 0x0000FFFF),      			\
        .base15_0                   = (((uint32_t)base)   & 0x0000FFFF),      			\
                                                                                        \
        .base23_16                  = (char) ((((uint32_t)base) >> 16) & 0x000000FF),   \
        .type                       = (char) (((uint16_t)flag) & 0x000000FF), 			\
                                                                                        \
        .limit19_16_and_flags       = ((((uint32_t)limit) >> 16) & 0x0000000F) |        \
                                        (uint8_t) (((uint16_t)flag)   >> 8),  			\
        .base31_24                  = (char) ((((uint32_t)base)  >> 24) & 0x000000FF)   \
    }

#pragma pack (1)

    struct gdt_descriptor {
        /**
         * @brief Bits 0..15 of the limit field.
         * 
         * Ignored in 64-bit mode.
         */
        uint16_t limit15_0;
        
        /**
         * @brief Bits 0..15, followed by bits 16 to 23 of the base field.
         * Ignored in 64-bit mode except for FS and GS.
         */
        uint16_t base15_0;
        uint8_t base23_16;

        uint8_t type;

        /**
         * @brief Holds bits 16..19 of the limit field and the segment flags.
         * The bits 16..19 of the limit field are ignored in 64-bit mode.
         */
        uint8_t limit19_16_and_flags;

        /**
         * @brief Bits 24..31 of the base field.
         * Ignored in 64-bit mode except for FS and GS.
         */
        uint8_t base31_24;
    };

    struct tss {
        uint32_t reserved0;
        uint64_t rsp0;
        uint64_t rsp1;
        uint64_t rsp2;
        uint64_t reserved1;
        uint64_t ist1;
        uint64_t ist2;
        uint64_t ist3;
        uint64_t ist4;
        uint64_t ist5;
        uint64_t ist6;
        uint64_t ist7;
        uint64_t reserved2;
        uint16_t reserved3;
        uint16_t iopb_offset;
    };
    extern struct tss tss;

    struct __attribute__((aligned(4096))) thornhill_gdt_table {
        struct gdt_descriptor null_descriptor;
        struct gdt_descriptor kernel_code;
        struct gdt_descriptor kernel_data;
        struct gdt_descriptor null_descriptor_2;
        struct gdt_descriptor user_data;
        struct gdt_descriptor user_code;
        struct gdt_descriptor ovmf_data;
        struct gdt_descriptor ovmf_code;
        struct gdt_descriptor tss_low;
        struct gdt_descriptor tss_high;
    };
    extern struct thornhill_gdt_table thornhill_gdt_table;

    struct table_ptr {
        uint16_t limit;
        uint64_t base;
    };

#pragma pack ()

#endif