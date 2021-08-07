#include "memory.h"

// EFI_STATUS THBPrepareHandoffMemoryMap(ThornhillHandoff* HandoffData){

    // EFI_STATUS Status;

    // /* Get the memory map from the firmware. */
    // EFI_MEMORY_DESCRIPTOR* Map = NULL;
    // UINTN MapSize, MapKey;
    // UINTN DescriptorSize;
    // UINT32 DescriptorVersion;

    // Status = ST->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize,
    //                                         &DescriptorVersion);
    // Status = ST->BootServices->AllocatePool(EfiLoaderData, MapSize, (void**) &Map);
    // Status = ST->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize,
    //                                         &DescriptorVersion);
    // if (EFI_ERROR(Status))
    //     return Status;

    // /* Calculate basic info about the map (start, end, size, etc.) */
    // UINTN* MapStart             = (UINTN*) Map;
    // UINTN* MapEnd               = MapStart + MapSize;
    // UINTN SegmentCount          = MapSize / DescriptorSize;
    // UINTN HandoffSegmentSize    = sizeof(HandoffMemorySegment);
    // UINTN HandoffMapSize        = SegmentCount * HandoffSegmentSize;

    // /* Allocate memory for the handoff memory map. */
    // HandoffMemorySegment** MemorySegments;
    // Status = ST->BootServices->AllocatePool(
    //     EfiLoaderData,
    //     HandoffMapSize,
    //     (void**)&MemorySegments
    // );
    // if (EFI_ERROR(Status))
    //     return Status;

    // /* Set the map key. */
    // do {
    //     Status = ST->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize,
    //                                             &DescriptorVersion);
    //     if (Status == EFI_BUFFER_TOO_SMALL) {
    //         Map = AllocatePool(MapSize + 1);
    //         Status = ST->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize,
    //                                                 &DescriptorVersion);
    //     }
    // } while (EFI_ERROR(Status));
    // *MapKeyPtr = MapKey;

    // /* Now write the handoff memory map. */
    // HandoffMemoryMap* HandoffMap = &(HandoffData->memoryMap);
    // HandoffMap->mapSize = HandoffMapSize;
    // HandoffMap->segments = MemorySegments;

    // for (UINTN SegmentIndex = 0; SegmentIndex < SegmentCount; SegmentIndex++) {
    //     EFI_MEMORY_DESCRIPTOR* Descriptor =
    //         (EFI_MEMORY_DESCRIPTOR*) (MapStart + (SegmentIndex * DescriptorSize));
    //     HandoffMemorySegment* HandoffSegment =
    //         (HandoffMemorySegment*) MemorySegments[SegmentIndex];

    //     // HandoffSegment->memoryType = THBUefiToThornhillMemoryType(Descriptor->Type);
    //     //HandoffSegment->physicalBaseAddress = Descriptor->PhysicalStart;
    //     //HandoffSegment->pageCount = Descriptor->NumberOfPages;
    // }

    // return Status;

    //}


ThornhillMemoryType THBUefiToThornhillMemoryType(EFI_MEMORY_TYPE memoryType) {

    switch (memoryType) {
        case EfiLoaderCode:
        case EfiLoaderData:
        case EfiBootServicesCode:
        case EfiBootServicesData:
        case EfiConventionalMemory:
            return THAvailableMemory;

        case EfiRuntimeServicesCode:
        case EfiRuntimeServicesData:
        case EfiACPIMemoryNVS:
        case EfiPalCode:
            return THReservedMemory;

        case EfiReservedMemoryType:
            return THUnusedMemory;

        case EfiUnusableMemory:
            return THBadMemory;

        case EfiMemoryMappedIO:
        case EfiMemoryMappedIOPortSpace:
            return THIOMemory;

        case EfiACPIReclaimMemory:
            return THACPIReclaimMemory;

        default:
            return THUnusedMemory;
    }
}