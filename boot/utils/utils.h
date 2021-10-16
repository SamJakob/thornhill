#include <efi.h>
#include <efidef.h>
#include <efilib.h>

typedef struct {
    EFI_MEMORY_DESCRIPTOR* Map;
    UINTN MapSize, MapKey;
    UINTN DescriptorSize;
    UINT32 DescriptorVersion;
} PreBootMemoryMap;

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
