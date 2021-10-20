#include <stddef.h>
#include <stdint.h>

#pragma once

typedef enum {
    /**
     * @brief  Memory that is available for the system to use.
     * @note   All usable memory will be handed over as THAvailableMemory.
     * @retval None
     */
    THAvailableMemory,

    /**
     * @brief  Memory that is reserved for the UEFI.
     * @note
     * @retval None
     */
    THReservedMemory,

    /**
     * @brief  Memory that is retained for the kernel.
     * @note
     * @retval None
     */
    THKernelMemory,

    /**
     * @brief  Memory that is reserved and which cannot be used by the system.
     * @note
     * @retval None
     */
    THUnusedMemory,

    /**
     * @brief  Memory that has failed tests and is considered unreliable for use.
     * @note
     * @retval None
     */
    THBadMemory,

    /**
     * @brief  Memory that is reserved by the UEFI for IO.
     * @note
     * @retval None
     */
    THIOMemory,

    /**
     * @brief  ACPI Reclaim Memory.
     * @note
     * @retval None
     */
    THACPIReclaimMemory
} ThornhillMemoryType;

typedef struct {

    ThornhillMemoryType memoryType;
    uint64_t physicalBaseAddress;
    uint64_t pageCount;

} HandoffMemorySegment;

typedef struct {

    size_t mapSize;
    HandoffMemorySegment* segments;

} HandoffMemoryMap;
