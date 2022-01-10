#include <efi.h>

#define EFI_TIMER_PERIOD_MICROSECONDS(Microseconds)     (Microseconds * 10)
#define EFI_TIMER_PERIOD_MILLISECONDS(Milliseconds)     (Milliseconds * 10000)
#define EFI_TIMER_PERIOD_SECONDS(Seconds)               (Seconds * 10000000)

/**
 * Attempt to sleep for the specified duration.
 * Returns the status as returned by internal UEFI calls (e.g., to SetTimer).
 *
 * You should use the THB_SLEEP_TIMER_PERIOD_MILLISECONDS or THB_SLEEP_TIMER_PERIOD_SECONDS
 * macros. This duration is accurate to 100 nanoseconds (ns).
 *
 * @param Duration The maximum duration to sleep until.
 * @param PressedKey A pointer to an integer that should contain the ScanCode for the pressed key,
 * if the user presses a key to skip sleep. This is set to 0 if PressedKey is null.
 * @param SkipKeyCodesCount The number of SkipKeyCodes set. If this value underestimates, excess
 * values will be ignored and if it overestimates, buffer overflows may occur.
 * @param SkipKeyCodes If not null, a pointer to an array of key code(s) to allow the user to
 * press to avoid sleeping.
 * @return EFI_SUCCESS, or the first error caused during the sleep procedure.
 */
EFI_STATUS THBSleep(
    UINTN Duration,
    UINT16* PressedKey,
    UINT16 SkipKeyCodesCount,
    const UINT16 SkipKeyCodes[]
);
