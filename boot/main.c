#include <stdbool.h>
#include <stddef.h>

#include <elf.h>

#include <efi.h>
#include <efilib.h>
#include <efivar.h>
#include <efidef.h>

#include "display/logging.h"
#include "loader/loader.h"
#include "utils/utils.h"

#include "handoff/handoff.h"
#include "handoff/memory/memory.h"

#define KERNEL_FILENAME L"kernel"

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
    Status = THBPrintMessage(L"Loading kernel...");
    Status = THBLoadKernel(Kernel, &KernelHeader);
    if (EFI_ERROR(Status)) {
        return THBErrorMessage(L"Failed to load kernel.", &Status);
    }

    /* Initialize the appropriate UEFI graphics mode for the OS. */
    Status = THBPrintMessage(L"Initializing graphics protocol...");
    EFI_GRAPHICS_OUTPUT_PROTOCOL* Graphics;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* GraphicsOutputInfo;
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
    Status = THBPrintMessage(L"Bootstrapping kernel...");
    ThornhillHandoff HandoffData;
    THBPrepareHandoffData(&HandoffData, Graphics);

    /* We're ready! Let's exit boot services, grab the new memory map
        and do this thing! */
    Status = THBPrintMessage(L"Starting...");

    PreBootMemoryMap MemoryMap;
    Status = THBAttemptExitBootServices(ImageHandle, &MemoryMap);
    if (EFI_ERROR(Status)) {
        return THBErrorMessage(L"Failed to boot.", &Status);
    }

    HandoffMemorySegment HandoffMemorySegments[
        (MemoryMap.MapSize / MemoryMap.DescriptorSize)
    ];
    {
        for (
            UINTN SegmentIndex = 0;
            SegmentIndex < (MemoryMap.MapSize / MemoryMap.DescriptorSize);
            SegmentIndex++
        ) {
            EFI_MEMORY_DESCRIPTOR* EfiDescriptor =
                (EFI_MEMORY_DESCRIPTOR*)(MemoryMap.Map +
                                         (SegmentIndex * MemoryMap.DescriptorSize));

            HandoffMemorySegments[SegmentIndex].memoryType = THBUefiToThornhillMemoryType(EfiDescriptor->Type);
            HandoffMemorySegments[SegmentIndex].pageCount = EfiDescriptor->NumberOfPages;
            HandoffMemorySegments[SegmentIndex].physicalBaseAddress = EfiDescriptor->PhysicalStart;
        }
    }

    HandoffData.memoryMap.mapSize =
        (MemoryMap.MapSize / MemoryMap.DescriptorSize) * sizeof(HandoffMemorySegment);
    HandoffData.memoryMap.segments = HandoffMemorySegments;

    /* Jump to kernel start! */
    ((void (*)(void*)) KernelHeader.e_entry)(
        (void*) &HandoffData
    );

    return EFI_SUCCESS;

}