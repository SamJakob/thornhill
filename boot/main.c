#include <stdbool.h>
#include <stddef.h>

#include <elf.h>
#include <efi.h>
#include <efilib.h>
#include <efivar.h>

#include "handoff/handoff.h"
#include "clock.h"

#include "../config/system.h"

const char* memoryTypes[] = {
    L"EfiReservedMemoryType",           // (unused)
    L"EfiLoaderCode",                   // (available)
    L"EfiLoaderData",                   // (available)
    L"EfiBootServicesCode",             // (available)
    L"EfiBootServicesData",             // (available)
    L"EfiRuntimeServicesCode",          // (reserved)
    L"EfiRuntimeServicesData",          // (reserved)
    L"EfiConventionalMemory",           // (available)
    L"EfiUnusableMemory",               // (bad memory)
    L"EfiACPIReclaimMemory",            // (reserved until ACPI enabled)
    L"EfiACPIMemoryNVS",                // (reserved)
    L"EfiMemoryMappedIO",               // (IO)
    L"EfiMemoryMappedIOPortSpace",      // (IO)
    L"EfiPalCode",                      // (reserved)
    L"EfiMaxMemoryType"
};

ThornhillMemoryType uefiToThornhillMemoryType(EFI_MEMORY_TYPE memoryType) {

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

int mem_compare(const void *aptr, const void *bptr, size_t n) {
    const unsigned char *a = aptr, *b = bptr;

    for (size_t i = 0; i < n; i++) {
        if (a[i] < b[i]) return -1;
        else if (a[i] > b[i]) return 1;
    }

    return 0;
}

EFI_STATUS ShowThornhillBootError(EFI_SYSTEM_TABLE *SystemTable, CHAR16* ErrorMessage, EFI_STATUS Status) {

    EFI_INPUT_KEY Key;

    // Print error message.
    SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16*) L"\r\n\r\n");
    SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16*) L"--> ");
    SystemTable->ConOut->OutputString(SystemTable->ConOut, ErrorMessage);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16*) L"\r\n");

    SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16*) L"=============== [ THORNHILL ] ============\r\n");
    SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16*) L"An error has occurred that prevented the  \r\n");
    SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16*) L"system from loading.                      \r\n");
    SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16*) L"==========================================\r\n");

    SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16*) L"\r\n");

    SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16*) L"Press any key to attempt to boot from another device.\r\n");

    // Wait for keypress to continue.
    Status = SystemTable->ConIn->Reset(SystemTable->ConIn, FALSE);
    if (EFI_ERROR(Status)) return Status;

    while ((Status = SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key)) == EFI_NOT_READY);
    return Status;

}

/** BEGIN: libthornhill - time */

uint8_t readByteFromPort (unsigned short port) {
    uint8_t result;

    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

void writeByteToPort (unsigned short port, uint8_t data) {
    __asm__("out %%al, %%dx" : : "a" (data), "d" (port));
}

uint8_t bcdToBinary(uint8_t bcd) {
    return ((bcd & 0xF0) >> 1) + ( (bcd & 0xF0) >> 3) + (bcd & 0xf);
}

uint8_t readCMOSRegister (unsigned short cmosRegister, bool bBinaryMode) {
    
    const bool NMIDisableBit = 0b1;
    writeByteToPort(0x70, (NMIDisableBit << 7) | cmosRegister);

    if (!bBinaryMode) return bcdToBinary(readByteFromPort(0x71));
    else return readByteFromPort(0x71);

}

/**
 * @brief  Reads the current time from the system RTC.
 * @note   DO NOT, **DO NOT** use this method alone to read the system time.
 *         This method alone does not ensure that the RTC is being updated,
 *         meaning that reading this could result in dodgy or inconsistent
 *         value such as reading any of 8:59, 8:60, 8:00 or 9:00 at 9:00am.
 * @retval ThornhillSystemTime
 */
struct ThornhillSystemTime _performTimeRead() {

    uint8_t statusRegisterB = readCMOSRegister(0x0B, true);
    bool b24HourFormat = (statusRegisterB & 0b010);
    bool bBinaryMode   = (statusRegisterB & 0b100);

    uint16_t century = readCMOSRegister(0x32, bBinaryMode);
    
    struct ThornhillSystemTime time;

    time.year        =       readCMOSRegister(0x09, bBinaryMode);
    time.fullYear    =       (century != 0) ? (century * 100) + time.year : 0;

    time.month       =       readCMOSRegister(0x08, bBinaryMode);
    time.day         =       readCMOSRegister(0x07, bBinaryMode);

    time.hours       =       readCMOSRegister(0x04, bBinaryMode);
    time.minutes     =       readCMOSRegister(0x02, bBinaryMode);
    time.seconds     =       readCMOSRegister(0x00, bBinaryMode);

    // Handle 24-hour time.
    if (b24HourFormat && time.hours > 12) {
        time.isPM = true;
        time.hours -= 12;
    }

    if (b24HourFormat && time.hours == 0) time.hours = 12;

    // Handle 12-hour time.
    if (!b24HourFormat) {
        if (time.hours &= 0x80) time.isPM = true;
        time.hours &= 0b01111111; // (Mask pm bit off.)
    }

    return time;

}

bool isRTCUpdateInProgress() {
    uint8_t statusRegisterA = readCMOSRegister(0x0A, false);
    return statusRegisterA &= 0x40;
}

/**
 * @brief  Reads the current system time from the RTC.
 * @note   See: https://wiki.osdev.org/CMOS#RTC_Update_In_Progress
 * @retval ThornhillSystemTime
 */
struct ThornhillSystemTime readTime() {

    struct ThornhillSystemTime time;

    while (isRTCUpdateInProgress()) {}
    time = _performTimeRead();

    while (isRTCUpdateInProgress()) {}
    if (compareSystemTime(time, _performTimeRead())) return time;
    else return readTime();

}

/** END: libthornhill - time **/

void LogBootMessage(EFI_SYSTEM_TABLE *SystemTable, CHAR16* Message) {

    struct ThornhillSystemTime systemTime = readTime();
    Print(
        (CHAR16*) L"[%d/%02d/%d %d:%02d:%02d%s] %s\r\n",
        systemTime.month,
        systemTime.day,
        systemTime.fullYear,

        systemTime.hours,
        systemTime.minutes,
        systemTime.seconds,
        
        systemTime.isPM ? L"PM" : L"AM",
        Message
    );

}

EFI_STATUS efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {

    InitializeLib(ImageHandle, SystemTable);

    EFI_STATUS Status;

    /* Print some stuff so we know our UEFI app is running... */
    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
    Status = SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16*) L"   /$$$$$$$$ /$$                                     /$$       /$$ /$$ /$$   \r\n");
    Status = SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16*) L"  |__  $$__/| $$                                    | $$      |__/| $$| $$   \r\n");
    Status = SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16*) L"     | $$   | $$$$$$$   /$$$$$$   /$$$$$$  /$$$$$$$ | $$$$$$$  /$$| $$| $$   \r\n");
    Status = SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16*) L"     | $$   | $$__  $$ /$$__  $$ /$$__  $$| $$__  $$| $$__  $$| $$| $$| $$   \r\n");
    Status = SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16*) L"     | $$   | $$  \\ $$| $$  \\ $$| $$  \\__/| $$  \\ $$| $$  \\ $$| $$| $$| $$   \r\n");
    Status = SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16*) L"     | $$   | $$  | $$| $$  | $$| $$      | $$  | $$| $$  | $$| $$| $$| $$   \r\n");
    Status = SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16*) L"     | $$   | $$  | $$|  $$$$$$/| $$      | $$  | $$| $$  | $$| $$| $$| $$   \r\n");
    Status = SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16*) L"     |__/   |__/  |__/ \\______/ |__/      |__/  |__/|__/  |__/|__/|__/|__/   \r\n");
    Status = SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16*) L"                                                                             \r\n");
    Status = SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16*) L"                                                                             \r\n");

    Status = SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16*) L"// Howdy!\r\n\r\n");
    LogBootMessage(SystemTable, L"Initializing system, please wait...");

    /* Open the kernel file from the device the UEFI app was loaded from. */
    LogBootMessage(SystemTable, L"Locating kernel...");
    EFI_FILE* Kernel;
    {
        EFI_HANDLE_PROTOCOL HandleProtocol = SystemTable->BootServices->HandleProtocol;

        EFI_LOADED_IMAGE_PROTOCOL* LoadedImage;
        HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, (void**) &LoadedImage);

        EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSystem;
        HandleProtocol(LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**) &FileSystem);

        EFI_FILE* Root;
        FileSystem->OpenVolume(FileSystem, &Root);

        Status = Root->Open(Root, &Kernel, L"kernel", EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
        if (EFI_ERROR(Status)) {
            return ShowThornhillBootError(SystemTable, L"Failed to locate system kernel!", Status);
        }
    }


    /* Load the ELF header from the kernel file. */
    Elf64_Ehdr kernelHeader;
    {
        UINTN FileInfoSize;
        EFI_FILE_INFO* FileInfo;

        Kernel->GetInfo(Kernel, &gEfiFileInfoGuid, &FileInfoSize, NULL);
        SystemTable->BootServices->AllocatePool(EfiLoaderData, FileInfoSize, (void**) &FileInfo);
        Kernel->GetInfo(Kernel, &gEfiFileInfoGuid, &FileInfoSize, (void**) &FileInfo);

        UINTN size = sizeof(kernelHeader);
        Kernel->Read(Kernel, &size, &kernelHeader);
    }


    /* Verify the binary */
    LogBootMessage(SystemTable, L"Checking kernel...");
    if (
        mem_compare(&kernelHeader.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 ||
        kernelHeader.e_ident[EI_CLASS] != ELFCLASS64 ||
        kernelHeader.e_ident[EI_DATA] != ELFDATA2LSB ||
        kernelHeader.e_machine != EM_X86_64 || // AMD64?
        kernelHeader.e_version != EV_CURRENT
    ) {
        return ShowThornhillBootError(SystemTable, (CHAR16*) L"The kernel is not compatible with your machine.\r\n", -1);
    }


    /* Load the segment headers */
    Elf64_Phdr* pHeaders;
    {
        Kernel->SetPosition(Kernel, kernelHeader.e_phoff);
        UINTN size = kernelHeader.e_phnum * kernelHeader.e_phentsize;

        SystemTable->BootServices->AllocatePool(EfiLoaderData, size, (void**) &pHeaders);
        Kernel->Read(Kernel, &size, pHeaders);
    }


    /* Load the kernel binary */
    LogBootMessage(SystemTable, L"Loading kernel...");
    for (
        Elf64_Phdr* pHeader = pHeaders;
        (char*) pHeader < (char*) pHeaders + kernelHeader.e_phnum * kernelHeader.e_phentsize;
        pHeader = (Elf64_Phdr*) ((char*) pHeader + kernelHeader.e_phentsize)
    ) {
        switch (pHeader->p_type) {
            case PT_LOAD: {
                int pages = (pHeader->p_memsz + 0x1000 - 1) / 0x1000;
                Elf64_Addr segment = pHeader->p_paddr;
                SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);

                Kernel->SetPosition(Kernel, pHeader->p_offset);
                UINTN size = pHeader->p_filesz;
                Kernel->Read(Kernel, &size, (void*) segment);
                break;
            }
        }
    }

    if (EFI_ERROR(Status)) {
        return ShowThornhillBootError(SystemTable, (CHAR16*) L"Failed to load kernel.\r\n", Status);
    }

    /* Get the memory map from the firmware */
    EFI_MEMORY_DESCRIPTOR* Map = NULL;
    UINTN MapSize, MapKey;
    UINTN DescriptorSize;
    UINT32 DescriptorVersion;
    {
        SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
        SystemTable->BootServices->AllocatePool(EfiLoaderData, MapSize, (void**) &Map);
        SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
    }

    if (EFI_ERROR(Status)) {
        return ShowThornhillBootError(SystemTable, (CHAR16*) L"Failed to prepare memory map.\r\n", Status);
    }


    /* Locate Grand Old Protocol (:P) */
    LogBootMessage(SystemTable, L"Initializing graphics protocol...");
    EFI_GRAPHICS_OUTPUT_PROTOCOL* GOP;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* GraphicsOutputInfo;
    UINT32 CurrentModeNumber;

    {
        Status = SystemTable->BootServices->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, (void**) &GOP);

        // TODO: Allow some kind of resolution selection - or auto detection, somehow?
        /*
        UINT32 CurrentModeNumber = 0;

        UINTN SizeOfModeInfo;
        EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *ModeInfo;

        while((GOP->QueryMode(GOP, CurrentModeNumber, &SizeOfModeInfo, &ModeInfo)) == EFI_SUCCESS) {
            Print(L"%d: %dx%d\n", CurrentModeNumber, ModeInfo->HorizontalResolution, ModeInfo->VerticalResolution);

            CurrentModeNumber++;
        }
        CurrentModeNumber--;
        */

        CurrentModeNumber = 21;

        if (EFI_ERROR(Status)) {
            return ShowThornhillBootError(SystemTable, (CHAR16*) L"Failed to initialize graphics protocol.\r\n", Status);
        }
    }

    GOP->SetMode(GOP, CurrentModeNumber);
    LogBootMessage(SystemTable, L"Reading memory map...\r\n");

    HandoffMemoryMap handoffMemoryMap;

    {
        UINTN *startOfMemoryMap       = (UINTN*) Map;
        UINTN *endOfMemoryMap         = startOfMemoryMap + MapSize;

        handoffMemoryMap.segmentSize = sizeof(HandoffMemorySegment);

        // The max possible segments of our handoff memory map is that
        // which is provided by the UEFI.
        // Though, it is likely less because we will amalgamate all
        // contigious available memory segments into one segment.
        size_t maxSegments = (endOfMemoryMap - startOfMemoryMap) / DescriptorSize;
        HandoffMemorySegment handoffMemorySegments[maxSegments * handoffMemoryMap.segmentSize];

        UINTN *offset = startOfMemoryMap;
        size_t segmentIndex = 0;

        while (offset < endOfMemoryMap) {
            EFI_MEMORY_DESCRIPTOR *descriptor = (EFI_MEMORY_DESCRIPTOR*) offset;

            // TODO: Merge contiguous available memory. (remember to change mapSize)

            handoffMemorySegments[segmentIndex].memoryType = uefiToThornhillMemoryType(descriptor->Type);
            handoffMemorySegments[segmentIndex].physicalBaseAddress = descriptor->PhysicalStart;
            handoffMemorySegments[segmentIndex].pageCount = descriptor->NumberOfPages;

            segmentIndex++;
            offset += DescriptorSize;
        }

        handoffMemoryMap.segments = handoffMemorySegments;
        handoffMemoryMap.mapSize = maxSegments * handoffMemoryMap.segmentSize;
    }


    /* ExitBootServices */
    LogBootMessage(SystemTable, L"Starting...\r\n");
    
    Status = SystemTable->BootServices->ExitBootServices(ImageHandle, MapKey);
    if (EFI_ERROR(Status)) {
        SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
        Status = SystemTable->BootServices->ExitBootServices(ImageHandle, MapKey);
    }
    if (EFI_ERROR(Status)) {
        SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
        Status = SystemTable->BootServices->ExitBootServices(ImageHandle, MapKey);
    }

    if (EFI_ERROR(Status)) {
        // Boot services could not be exited after 3 attempts.
        return ShowThornhillBootError(SystemTable, (CHAR16*) L"Failed to exit bootloader after 3 attempts.\r\n", Status);
    }

    /* Prepare handoff data. */
    ThornhillHandoff HOData;

    // -> MemoryMap
    HOData.memoryMap = handoffMemoryMap;

    // -> GOP
    Screen screen;
    screen.width = GOP->Mode->Info->HorizontalResolution;
    screen.height = GOP->Mode->Info->VerticalResolution;
    screen.pixels_per_scanline  = GOP->Mode->Info->PixelsPerScanLine;
    screen.frame_buffer_base = GOP->Mode->FrameBufferBase;
    screen.frame_buffer_size = GOP->Mode->FrameBufferSize;
    screen.pixel_format = GOP->Mode->Info->PixelFormat;
    HOData.screen = screen;

    /* Jump to start of kernel */
    ((void (*)(void*)) kernelHeader.e_entry)(
        (void*) &HOData
    );

    return EFI_SUCCESS;

}