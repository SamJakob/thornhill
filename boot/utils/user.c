#include "user.h"

#include <efilib.h>

typedef struct {
    // The number of elements in the SkipKeyCodes array.
    UINTN SkipKeyCodesCount;
    // The array of SkipKeyCodes.
    const UINT16* SkipKeyCodes;
    // Return value for PressedKey.
    UINT16* PressedKey;
} THBSleepDelegateContext;

VOID EFI_FUNCTION test(
    IN EFI_EVENT                    Event,
    IN THBSleepDelegateContext*     Context
) {
    EFI_INPUT_KEY PressedKey;
    EFI_STATUS Status = ST->ConIn->ReadKeyStroke(ST->ConIn, &PressedKey);

    if (!EFI_ERROR(Status)) {
        for (UINTN Index = 0; Index < Context->SkipKeyCodesCount; Index++) {
            if (PressedKey.ScanCode == Context->SkipKeyCodes[Index]) {
                *Context->PressedKey = PressedKey.ScanCode;
                ST->BootServices->SignalEvent(Event);
                return;
            }
        }
    }
}

EFI_STATUS THBSleep(
    UINTN Duration,
    UINT16* PressedKey,
    UINT16 SkipKeyCodesCount,
    const UINT16 SkipKeyCodes[]
) {

    EFI_STATUS Status = EFI_SUCCESS;
    if (PressedKey != NULL) *PressedKey = 0;

    // We should additionally expect SkipKeyCodes to allow the user to skip the sleep.
    if (SkipKeyCodes != NULL) {
        // The event triggered during WaitForEvent.
        UINTN EventTriggeredIndex;

        /**
         * The list of events to wait for. This will be passed to WaitForEvent.
         * - [0] is the user's key press event.
         * - [1] (set dynamically later) is a timer for the delay duration.
         */
        EFI_EVENT EventList[2];

        // Dynamically set the event for the key press.
        THBSleepDelegateContext Context = {
            SkipKeyCodesCount,
            SkipKeyCodes,
            PressedKey
        };
        Status = ST->BootServices->CreateEvent(
            EVT_NOTIFY_WAIT, TPL_NOTIFY,
            (EFI_EVENT_NOTIFY) test,
            (VOID*) &Context,
            &EventList[0]
        );
        if (EFI_ERROR(Status)) return Status;

        // Dynamically set the event for the timer.
        Status = ST->BootServices->CreateEvent(EVT_TIMER, TPL_CALLBACK, NULL, NULL, &EventList[1]);
        if (EFI_ERROR(Status)) return Status;

        // Set the duration of and start the timer.
        Status = ST->BootServices->SetTimer(EventList[1], TimerRelative, Duration);
        if (EFI_ERROR(Status)) {
            ST->BootServices->CloseEvent(EventList[0]);
            ST->BootServices->CloseEvent(EventList[1]);
            return Status;
        }

        // Wait for either the console input, or the timer.
        Status = ST->BootServices->WaitForEvent(2, EventList, &EventTriggeredIndex);
        if (EFI_ERROR(Status)) return Status;

        // Close the timer event.
        ST->BootServices->CloseEvent(EventList[0]);
        ST->BootServices->CloseEvent(EventList[1]);

        // If the timer expired ensure that it is indicated that no key was pressed.
        if (EventTriggeredIndex == 1) {
            if (PressedKey != NULL) *PressedKey = 0;
            return Status;
        }

        // Otherwise, an acceptable key was pressed.
        return Status;
    }

    // We aren't expecting anything, so we can just set a timer and wait.
    EFI_EVENT TimerEvent;
    Status = ST->BootServices->CreateEvent(EVT_TIMER, TPL_CALLBACK, NULL, NULL, &TimerEvent);
    if (EFI_ERROR(Status)) return Status;

    // Set the duration of and start the timer.
    Status = ST->BootServices->SetTimer(TimerEvent, TimerRelative, Duration);
    if (EFI_ERROR(Status)) {
        ST->BootServices->CloseEvent(TimerEvent);
        return Status;
    }

    UINTN Index = 0;
    ST->BootServices->WaitForEvent(1, &TimerEvent, &Index);

    ST->BootServices->CloseEvent(TimerEvent);
    return Status;

}
