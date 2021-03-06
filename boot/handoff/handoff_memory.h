#include <stddef.h>
#include <stdint.h>

#ifndef TH_BOOT_HANDOFF_MEM
#define TH_BOOT_HANDOFF_MEM

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
    
    HandoffMemorySegment* segments;
    size_t mapSize;
    size_t segmentSize;

} HandoffMemoryMap;

#endif