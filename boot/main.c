#include <stdbool.h>
#include <stddef.h>

#include <elf.h>

#include <efi.h>
#include <efilib.h>
#include <efidef.h>

#include "display/logging.h"
#include "loader/loader.h"
#include "loader/paging.h"

#include "handoff/handoff.h"
#include "handoff/memory/memory.h"

#include "config.h"

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {

    // Initializes the GNU EFI libraries. Performing various tasks, such as
    // defining a globally enabled ST variable to access the SystemTable.
    InitializeLib(ImageHandle, SystemTable);

    EFI_STATUS Status;
    Status = THBPrintBanner(true);
    Status = THBPrintMessage(L"Initializing system, please wait...");
    
    /* Open the kernel file from the device the UEFI app was loaded from. */
    Status = THBPrintMessage(L"Locating kernel...");
    EFI_FILE* Kernel = THBLoadKernelFile(&ImageHandle, KERNEL_FILENAME);
    if (Kernel == NULL) {
        return THBErrorMessage(
            L"Failed to locate system kernel on boot device.",
            NULL
        );
    }
    
    /* Inspect the kernel - ensure it's compatible with the system, etc. */
    Elf64_Ehdr KernelHeader;
    Status = THBPrintMessage(L"Checking kernel...");
    CHAR16* KernelErrorMessage = THBCheckKernel(Kernel, &KernelHeader);
    if (KernelErrorMessage != NULL) {
        return THBErrorMessage(KernelErrorMessage, NULL);
    }

    /* Load the kernel into memory. */
    THBKernelSymbols KernelSymbols;
    Status = THBPrintMessage(L"Loading kernel...");
    Status = THBLoadKernel(Kernel, &KernelHeader, &KernelSymbols);
    if (EFI_ERROR(Status)) {
        return THBErrorMessage(L"Failed to load kernel.", &Status);
    }

    /* Initialize the appropriate UEFI graphics mode for the OS. */
    Status = THBPrintMessage(L"Initializing graphics protocol...");
    EFI_GRAPHICS_OUTPUT_PROTOCOL* Graphics;
    // EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* GraphicsOutputInfo;
    {
        UINT32 CurrentGraphicsMode;

        // TODO: Allow resolution auto-detection or selection.
        Status = ST->BootServices->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, (void**) &Graphics);
        CurrentGraphicsMode = 21;

        Status = Graphics->SetMode(Graphics, CurrentGraphicsMode);

        if (EFI_ERROR(Status)) {
            return THBErrorMessage(L"Failed to initialize graphics protocol.", &Status);
        }

        ST->ConOut->ClearScreen(ST->ConOut);
    }

    /* Start handoff procedure so we can jump to kernel. */
    Status = THBPrintMessage(L"Preparing pre-boot data for kernel...");
    ThornhillHandoff HandoffData;
    THBPrepareHandoffData(&HandoffData, Graphics);

    /* Add page table entry for kernel base address */
    Status = THBSetupPaging(&KernelSymbols);
    if (EFI_ERROR(Status)) {
        return THBErrorMessage(L"Failed to set up kernel environment.", &Status);
    }

    /* We're ready! Let's exit boot services, grab the new memory map
        and do this thing! */
    Status = THBPrintMessage(L"Starting kernel pre-boot environment...");

    /* Exit boot services. */
    EFI_MEMORY_DESCRIPTOR* Map = NULL;
    UINTN MapSize, MapKey;
    UINTN DescriptorSize;
    UINT32 DescriptorVersion;

    // It is recommended to get the memory map a few (3-4) times, because there's
    // a catch-22 scenario, with respect to allocating the memory to store the
    // memory map (allocating memory for GetMemoryMap, can sometimes change the
    // memory map meaning it needs to be called again.)
    // To solve this, the solution is apparently just to call the method a few
    // times and kinda just hope for the best. So here we are.
    Status = ST->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
    Status = ST->BootServices->AllocatePool(EfiLoaderData, MapSize, (void**)&Map);
    Status = ST->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
    Status = ST->BootServices->AllocatePool(EfiLoaderData, MapSize, (void**)&Map);
    Status = ST->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
    Status = ST->BootServices->AllocatePool(EfiLoaderData, MapSize, (void**)&Map);
    Status = ST->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);

    if (EFI_ERROR(Status))
        return THBErrorMessage(L"An error occurred whilst preparing to exit the firmware pre-boot environment.", &Status);

    Status = ST->BootServices->ExitBootServices(ImageHandle, MapKey);

    if (EFI_ERROR(Status)) {
        return THBErrorMessage(L"Failed to exit the firmware pre-boot environment.", &Status);
    }

    /* Prepare hand-off data based on the memory map that we just fetched. */
    HandoffMemorySegment HandoffMemorySegments[
        (MapSize / DescriptorSize)
    ];
    {
        UINTN* StartOfMap = (UINTN*) Map;
        UINTN* EndOfMap = StartOfMap + MapSize;

        UINTN SegmentIndex = 0;
        UINTN* Offset = StartOfMap;
        EFI_MEMORY_DESCRIPTOR* EfiDescriptor = (EFI_MEMORY_DESCRIPTOR*) Offset;

        while (Offset < EndOfMap) {
            HandoffMemorySegments[SegmentIndex].memoryType =
                THBUefiToThornhillMemoryType(EfiDescriptor->Type);
            HandoffMemorySegments[SegmentIndex].pageCount = EfiDescriptor->NumberOfPages;
            HandoffMemorySegments[SegmentIndex].physicalBaseAddress = EfiDescriptor->PhysicalStart;

            Offset += DescriptorSize;
        }
    }

    HandoffData.memoryMap.mapSize =
        (MapSize / DescriptorSize) * sizeof(HandoffMemorySegment);
    HandoffData.memoryMap.segments = HandoffMemorySegments;

    /* Jump to kernel start! */
    ((void (*)(void*)) (KernelHeader.e_entry))(
        (void*) &HandoffData
    );

    return EFI_SUCCESS;

}