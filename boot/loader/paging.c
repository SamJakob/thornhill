#include "paging.h"

EFI_STATUS THBSetupPaging(THBKernelSymbols* KernelSymbols) {

    EFI_STATUS Status = EFI_SUCCESS;

    // Load the root page table from CR3.
    // The CR3 register holds the base address of the P4 page table.
    uint64_t* cr3;
    __asm__ __volatile__ (
        "movq %%cr3, %%rax \n\t"
        "movq %%rax, %0    \n\t"
        : "=m" (cr3)
        : /* no input */
        : "%rax"
    );
    cr3 = (uint64_t*) (((uint64_t) cr3 >> 12) << 12);

    // Allocate space for our kernel's offset-identity-mapped P3 table.
    uint64_t* Level3PageTable;
    Status = ST->BootServices->AllocatePages(
        AllocateAnyPages,
        EfiLoaderData,
        (((8 * 512) + EFI_PAGE_SIZE) - 1) / EFI_PAGE_SIZE,
        (EFI_PHYSICAL_ADDRESS*) &Level3PageTable
    );
    if (EFI_ERROR(Status)) return Status;
    // Copy the old P3 table into the new one.
    uint64_t* OldLevel3PageTable = (uint64_t*) (MASK_PAGE_ENTRY_ADDR((*cr3 + MASK_VADDR_P4_ADDR(KernelSymbols->KernelBaseAddress))));
    CopyMem(Level3PageTable, OldLevel3PageTable, 8 * 512);

    // To update the page table, we'll need to replace it with our own.
    // So, let's allocate space for the P4 table...
    uint64_t* Level4PageTable;
    Status = ST->BootServices->AllocatePages(
        AllocateAnyPages,
        EfiLoaderData,
        (((8 * 512) + EFI_PAGE_SIZE) - 1) / EFI_PAGE_SIZE,
        (EFI_PHYSICAL_ADDRESS*) &Level4PageTable
    );
    if (EFI_ERROR(Status)) return Status;

    // ...and copy the existing P4 table into our P4 table.
    CopyMem(Level4PageTable, cr3, 8 * 512);
    // ...finally, update the P4 to include our copied P3 table.
    Level4PageTable[0] = ((uint64_t) Level3PageTable) | 0b11;

    // Update the CR3 register to replace the previous page table
    // with our page table. This also has the effect of flushing the
    // TLB, but we'll do it again explicitly after making our
    // modifications.
    __asm__ __volatile__ (
        "movq %0,    %%cr3 \n\t"
        : /* no output */
        :  "r" ((uint64_t) Level4PageTable)
    );

    // Allocate space for the P2 tables.
    uint64_t* Level2PageTable;
    Status = ST->BootServices->AllocatePages(
        AllocateAnyPages,
        EfiLoaderData,
        (((8 * 512) + EFI_PAGE_SIZE) - 1) / EFI_PAGE_SIZE,
        (EFI_PHYSICAL_ADDRESS*) &Level2PageTable);
    if (EFI_ERROR(Status)) return Status;

    // Allocate space for the P1 tables.
    uint64_t* Level1PageTable;
    Status = ST->BootServices->AllocatePages(
        AllocateAnyPages,
        EfiLoaderData,
        (((8 * 512 * 512) + EFI_PAGE_SIZE) - 1) / EFI_PAGE_SIZE,
        (EFI_PHYSICAL_ADDRESS*) &Level1PageTable);
    if (EFI_ERROR(Status)) return Status;

    // Populate the P2 table.
    for (int i = 0; i < 512; i++) {
        uint64_t* Level1Base = Level1PageTable + (i * 512);

        // Populate the P1 table for this P2 entry.
        for (int j = 0; j < 512; j++) {
            Level1Base[j] = ((j << 12) + ( i << 21 ))
                    /* P1 flags: writeable, present. */
                    | 0b11;
        }

        // Create the P2 entry.
        Level2PageTable[i] = ((uint64_t) Level1Base)
                      /* P2 flags: writeable, present. */
                      | 0b11;
    }

    // Write our P2 table into our P3 table.
    Level3PageTable[
        MASK_VADDR_P3_ADDR(KernelSymbols->KernelBaseAddress)
    ] = ((uint64_t)Level2PageTable)
                     /* P3 flags: writeable, present. */
                     | 0b11;

    // Apply our changes:
    // Flush the Transatlantic Lobster Buffet.
    __asm__ __volatile__("movq %cr3, %rax \n\t"
                         "movq %rax, %cr3 \n\t");

    // ...and we're good to go!
    return Status;

}