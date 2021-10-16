#include "loader.h"

#include "../utils/utils.h"

EFI_FILE* THBLoadKernelFile(EFI_HANDLE* Handle, CHAR16* FileName) {
    EFI_STATUS Status;
    EFI_HANDLE_PROTOCOL HandleProtocol = ST->BootServices->HandleProtocol;
    
    EFI_LOADED_IMAGE_PROTOCOL* LoadedImage;
    Status = HandleProtocol(*Handle, &gEfiLoadedImageProtocolGuid, (void**) &LoadedImage);

    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSystem;
    HandleProtocol(LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**) &FileSystem);

    EFI_FILE* Root;
    FileSystem->OpenVolume(FileSystem, &Root);

    EFI_FILE* Kernel;
    Status = Root->Open(Root, &Kernel, FileName, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);

    if (EFI_ERROR(Status))
        return NULL;
    else
        return Kernel;
}

CHAR16* THBCheckKernel(EFI_FILE* Kernel, Elf64_Ehdr* KernelHeader) {

    /* Load the ELF header from the Kernel file */
    UINTN KernelHeaderSize = sizeof(*KernelHeader);
    EFI_STATUS Status = Kernel->Read(Kernel, &KernelHeaderSize, KernelHeader);
    if (EFI_ERROR(Status)) {
        return L"The kernel is invalid or corrupted.";
    }

    /* Verify the kernel binary */
    if (
        mem_compare(&(KernelHeader->e_ident[EI_MAG0]), ELFMAG, SELFMAG) != 0 ||
        KernelHeader->e_ident[EI_CLASS] != ELFCLASS64 ||
        KernelHeader->e_ident[EI_DATA] != ELFDATA2LSB ||
        KernelHeader->e_machine != EM_X86_64 ||
        KernelHeader->e_version != EV_CURRENT
    )
        return L"The kernel is not compatible with your machine.";

    return NULL;
    
}

EFI_STATUS THBLoadKernel(EFI_FILE* Kernel, Elf64_Ehdr* KernelHeader) {

    // The status of the load operation.
    EFI_STATUS Status;

    /* Load the program and segment headers. */
    Elf64_Phdr* KernelProgramHeaders;

    // Seek to the program headers offset in the ELF file.
    Kernel->SetPosition(Kernel, KernelHeader->e_phoff);

    // Allocate a memory pool for each of the program sections of the
    // kernel file.
    UINTN KernelProgramHeaderTableSize = KernelHeader->e_phnum * KernelHeader->e_phentsize;
    ST->BootServices->AllocatePool(EfiLoaderData, KernelProgramHeaderTableSize,
                                   (void**)&KernelProgramHeaders);
    Status = Kernel->Read(Kernel, &KernelProgramHeaderTableSize, KernelProgramHeaders);
    if (EFI_ERROR(Status))
        return Status;

    /* Load the kernel binary into memory. */
    for (
        // Create a temporary pointer for the current KernelProgramHeader.
        Elf64_Phdr* KernelProgramHeaderEntry = KernelProgramHeaders;
        // Ensure we don't proceed past the program header table.
        (char*)KernelProgramHeaderEntry <
        (char*)KernelProgramHeaders + KernelProgramHeaderTableSize;
        // Loop over each program section header
        // (i.e. kernel program header entry).
        KernelProgramHeaderEntry =
            (Elf64_Phdr*)((char*)KernelProgramHeaderEntry + KernelHeader->e_phentsize)
    ) {
        switch (KernelProgramHeaderEntry->p_type) {
            // Loop over the segments until we find a loadable program
            // segment.
            case PT_LOAD: {
                // Determine how many pages of EFI_PAGE_SIZE are needed for the section we're
                // currently allocating.
                UINTN Pages = ((KernelProgramHeaderEntry->p_memsz + EFI_PAGE_SIZE) - 1) / EFI_PAGE_SIZE;

                // Once our pages have been allocated in memory, this will be the contiguous base
                // address of the newly allocated memory pages.
                Elf64_Addr Segment = KernelProgramHeaderEntry->p_vaddr;

                // Allocate the required number of pages.
                EFI_MEMORY_TYPE Type =
                    // If the segment is executable and not writeable, mark it as EfiLoaderCode,
                    // otherwise mark it as EfiLoaderData.
                    (KernelProgramHeaderEntry->p_flags & PF_X) != 0 && (KernelProgramHeaderEntry->p_flags & PF_W) == 0
                        ? EfiLoaderCode : EfiLoaderData;
                Status = ST->BootServices->AllocatePages(AllocateAddress, Type, Pages, &Segment);

                if (EFI_ERROR(Status))
                    return Status;

                // Seek to the offset of the current section in the file.
                Kernel->SetPosition(Kernel, KernelProgramHeaderEntry->p_offset);
                // Identify the size of the current section.
                UINTN Size = KernelProgramHeaderEntry->p_filesz;
                // Finally, load the current section into memory.
                Status = Kernel->Read(Kernel, &Size, (void*) Segment);

                if (EFI_ERROR(Status))
                    return Status;

                break;
            }
        }
    }

    return Status;
}