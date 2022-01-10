#include <stdbool.h>
#include <stddef.h>

#include <elf.h>

#include <efi.h>
#include <efilib.h>
#include <efidef.h>

#include "display/logging.h"
#include "display/logo.h"
#include "loader/loader.h"
#include "loader/paging.h"
#include "utils/file.h"
#include "utils/user.h"

#include "handoff/handoff.h"
#include "handoff/memory/memory.h"

#include "config.h"

__attribute__((unused)) EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {

    // Initializes the GNU EFI libraries. Performing various tasks, such as
    // defining a globally enabled ST variable to access the SystemTable.
    InitializeLib(ImageHandle, SystemTable);

    EFI_STATUS Status;
    Status = THBPrintBanner(true);
    Status = THBPrintMessage(L"Initializing system, please wait...");

    /* Locate Graphics Output Protocol. */
    EFI_GRAPHICS_OUTPUT_PROTOCOL* Graphics;
    Status = ST->BootServices->LocateProtocol(
        &gEfiGraphicsOutputProtocolGuid, NULL, (void**) &Graphics
    );
    if (EFI_ERROR(Status)) {
        return THBErrorMessage(
            L"Failed to locate Graphics Output Protocol.",
            NULL
        );
    }

    /* Initialize the appropriate UEFI graphics mode for the OS. */
    Status = THBPrintMessage(L"Configuring graphics output protocol...");
    // EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* GraphicsOutputInfo;
    {
        UINT32 CurrentGraphicsMode;

        // TODO: Allow resolution auto-detection or selection.
        CurrentGraphicsMode = 21;
        Status = Graphics->SetMode(Graphics, CurrentGraphicsMode);

        if (EFI_ERROR(Status)) {
            return THBErrorMessage(L"Failed to initialize graphics output protocol.", &Status);
        }

        ST->ConOut->ClearScreen(ST->ConOut);
    }

#if !SHOW_INFO_MESSAGES
    /* Render the background and boot logo. */
    EFI_FILE* BootLogo = THBLocateFile(&ImageHandle, LOGO_FILENAME);
    // Simply fills the background with a solid color.
    // This doubles as a way of checking if GOP is indeed functioning.
    THBRenderBackground(Graphics);
    // Loads the boot logo as specified in the config from the boot device and then
    // renders it on screen.
    THBRenderBootLogo(BootLogo, Graphics);
#endif
    
    /* Open the kernel file from the device the UEFI app was loaded from. */
    Status = THBPrintMessage(L"Locating kernel...");
    EFI_FILE* Kernel = THBLocateFile(&ImageHandle, KERNEL_FILENAME);
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

    /* Sleep for BOOT_DELAY_SECONDS (if it's greater than 0). */
#if BOOT_DELAY_SECONDS > 0 && !SHOW_INFO_MESSAGES
    UINT16 SleepSkipKeyCodes[] = { SCAN_ESC };
    UINT16 PressedKey;
    Status = THBSleep(
        EFI_TIMER_PERIOD_SECONDS(BOOT_DELAY_SECONDS),
        &PressedKey,
        1, SleepSkipKeyCodes
    );
#endif

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
    UINTN MapSize = 0, MapKey = 0;
    UINTN DescriptorSize = 0;
    UINT32 DescriptorVersion = 0;

    // Compliant UEFI implementations will immediately return EFI_BUFFER_TOO_SMALL when
    // GetMemoryMap is called initially (assuming MapSize is set to 0 - which would be appropriate
    // as there's no way of knowing the correct map size straight away.)
    // However, we pass in a reference to our MapSize integer, which a compliant firmware will then
    // overwrite with an appropriate map size in bytes.
    Status = ST->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
    if (Status != EFI_BUFFER_TOO_SMALL) {
        return THBErrorMessage(L"Non-compliant UEFI implementation. Report to your local motherboard manufacturer.", &Status);
    }

    // Before increasing the MapSize, we can here allocate memory to hold our own memory map for
    // handing off to the kernel.
    HandoffMemorySegment* HandoffMemorySegments;
    Status = ST->BootServices->AllocatePool(
        EfiLoaderData,
        sizeof(HandoffMemorySegment) * (MapSize / DescriptorSize),
        (void**)&HandoffMemorySegments
    );

    // Because we're about to have to create a second memory map (containing only the information
    // the kernel needs in a preferred format and so the kernel needn't include all the EFI
    // headers) we add enough memory to the map for two additional descriptors.
    // (Requiring more would be an extremely unusually fragmented memory map, but to accommodate
    // as such, one can simply increase the constant size, or take multiple attempts to acquire
    // enough space for the memory map.)
    MapSize += (2 * DescriptorSize);

    // Finally, we attempt to allocate the necessary memory and then pass this newly allocated
    // buffer into GetMemoryMap.
    Status = ST->BootServices->AllocatePool(EfiLoaderData, MapSize, (void**)&Map);
    Status = ST->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);

    if (EFI_ERROR(Status))
        return THBErrorMessage(L"An error occurred whilst preparing to exit the firmware pre-boot environment.", &Status);

    /* Prepare hand-off data based on the memory map that we just fetched. */
    UINTN SegmentIndex = 0;
    {
        UINTN StartOfMap = (UINTN) Map;
        UINTN EndOfMap = StartOfMap + MapSize;
        UINTN Offset = StartOfMap;

        while (Offset < EndOfMap) {
            EFI_MEMORY_DESCRIPTOR* EfiDescriptor = (EFI_MEMORY_DESCRIPTOR*) Offset;

            HandoffMemorySegments[SegmentIndex].memoryType =
                THBUefiToThornhillMemoryType(EfiDescriptor->Type);
            HandoffMemorySegments[SegmentIndex].pageCount = EfiDescriptor->NumberOfPages;
            HandoffMemorySegments[SegmentIndex].physicalBaseAddress = EfiDescriptor->PhysicalStart;

            SegmentIndex++;
            Offset += DescriptorSize;
        }
    }

    HandoffData.memoryMap.mapSize = SegmentIndex;
    HandoffData.memoryMap.segments = HandoffMemorySegments;

    /* Exit boot services. */
    Status = ST->BootServices->ExitBootServices(ImageHandle, MapKey);

    if (EFI_ERROR(Status)) {
        return THBErrorMessage(
            L"Failed to exit the firmware pre-boot environment.",
            &Status
        );
    }

    /* Jump to kernel start! */
    ((void (*)(void*)) (KernelHeader.e_entry))(
        (void*) &HandoffData
    );

    return EFI_SUCCESS;

}
