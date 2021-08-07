#include <efi.h>
#include <efidef.h>
#include <efilib.h>

/**
 * @brief Compares memory of n length starting at aptr to memory of the same
 * length starting at bptr.
 * 
 * @param aptr The first memory base to compare.
 * @param bptr The second memory base to compare.
 * @param n The amount of bytes to compare.
 * @return int -1 if a bit of bptr is higher, 1 if it's lower, or 0 if all bits
 * were identical.
 */
int mem_compare(const void* aptr, const void* bptr, UINTN n);

/**
 * @brief Makes three attempts to exit boot services, returning the EFI_STATUS
 * from the internal calls which can then be checked to determine how or if
 * booting should proceed.
 * 
 * @param ImageHandle The EFI_HANDLE that references the current EFI structure.
 * @return EFI_STATUS The status as returned from the internal calls to exit
 * boot services.
 */
EFI_STATUS THBAttemptExitBootServices(EFI_HANDLE* ImageHandle);