#include "../config.h"
#include "logging.h"

EFI_STATUS THBPrintBanner(__attribute__((unused)) bool ShouldClearScreen) {

#if SHOW_INFO_MESSAGES

    EFI_STATUS Status;

    if (ShouldClearScreen) ST->ConOut->ClearScreen(ST->ConOut);
    Status = ST->ConOut->OutputString(ST->ConOut, (CHAR16*) L"   /$$$$$$$$ /$$                                     /$$       /$$ /$$ /$$   \r\n");
    Status = ST->ConOut->OutputString(ST->ConOut, (CHAR16*) L"  |__  $$__/| $$                                    | $$      |__/| $$| $$   \r\n");
    Status = ST->ConOut->OutputString(ST->ConOut, (CHAR16*) L"     | $$   | $$$$$$$   /$$$$$$   /$$$$$$  /$$$$$$$ | $$$$$$$  /$$| $$| $$   \r\n");
    Status = ST->ConOut->OutputString(ST->ConOut, (CHAR16*) L"     | $$   | $$__  $$ /$$__  $$ /$$__  $$| $$__  $$| $$__  $$| $$| $$| $$   \r\n");
    Status = ST->ConOut->OutputString(ST->ConOut, (CHAR16*) L"     | $$   | $$  \\ $$| $$  \\ $$| $$  \\__/| $$  \\ $$| $$  \\ $$| $$| $$| $$   \r\n");
    Status = ST->ConOut->OutputString(ST->ConOut, (CHAR16*) L"     | $$   | $$  | $$| $$  | $$| $$      | $$  | $$| $$  | $$| $$| $$| $$   \r\n");
    Status = ST->ConOut->OutputString(ST->ConOut, (CHAR16*) L"     | $$   | $$  | $$|  $$$$$$/| $$      | $$  | $$| $$  | $$| $$| $$| $$   \r\n");
    Status = ST->ConOut->OutputString(ST->ConOut, (CHAR16*) L"     |__/   |__/  |__/ \\______/ |__/      |__/  |__/|__/  |__/|__/|__/|__/   \r\n");
    Status = ST->ConOut->OutputString(ST->ConOut, (CHAR16*) L"                                                                             \r\n");
    Status = ST->ConOut->OutputString(ST->ConOut, (CHAR16*) L"                                                                             \r\n");
    Status = ST->ConOut->OutputString(ST->ConOut, (CHAR16*) L"// Howdy!\r\n\r\n");

    return Status;

#else

    return EFI_SUCCESS;

#endif

}

EFI_STATUS THBPrintMessage(__attribute__((unused)) CHAR16* Message) {

#if SHOW_INFO_MESSAGES

    // TODO: fetch time manually from the RTC.

    EFI_STATUS Status;
    EFI_TIME CurrentTime;

    Status = ST->RuntimeServices->GetTime(&CurrentTime, NULL);
    UINT8 CurrentHour = CurrentTime.Hour;
    bool isPM = false;

    if (CurrentHour >= 12) {
        isPM = true;
        CurrentHour -= 12;
    }

    if (CurrentHour == 0)
        CurrentHour = 12;

    Print(
        (CHAR16*) L"[BOOT] [%d/%02d/%d %d:%02d:%02d %s] %s\r\n",
        CurrentTime.Month,
        CurrentTime.Day,
        CurrentTime.Year,

        CurrentHour,
        CurrentTime.Minute,
        CurrentTime.Second,

        isPM ? L"PM" : L"AM",
        Message
    );

    return Status;

#else

    return EFI_SUCCESS;

#endif

}

EFI_STATUS THBErrorMessage(CHAR16* ErrorMessage, const EFI_STATUS* StatusCode) {

    EFI_INPUT_KEY Key;

    // Print error message.
    ST->ConOut->OutputString(ST->ConOut, L"\r\n\r\n");
    ST->ConOut->OutputString(ST->ConOut, L"(!) ");
    ST->ConOut->OutputString(ST->ConOut, ErrorMessage);
    ST->ConOut->OutputString(ST->ConOut, L"\r\n");
    if (StatusCode != NULL) {
        Print(L"(!) SYSTEM: %r", *StatusCode);
        ST->ConOut->OutputString(ST->ConOut, L"\r\n");
    }
    ST->ConOut->OutputString(ST->ConOut, L"\r\n");

    ST->ConOut->OutputString(ST->ConOut, L"=============== [ THORNHILL ] ============\r\n");
    ST->ConOut->OutputString(ST->ConOut, L"An error has occurred that prevented the  \r\n");
    ST->ConOut->OutputString(ST->ConOut, L"system from loading.                      \r\n");
    ST->ConOut->OutputString(ST->ConOut, L"==========================================\r\n");

    ST->ConOut->OutputString(ST->ConOut, L"\r\n");
    ST->ConOut->OutputString(ST->ConOut, L"Press any key to attempt to boot from another device.\r\n");

    
    // Wait for keypress to continue.
    EFI_STATUS Status = ST->ConIn->Reset(ST->ConIn, FALSE);
    if (EFI_ERROR(Status))
        return Status; // If there's an error doing *that*, just give up lol.

    while ((Status = ST->ConIn->ReadKeyStroke(ST->ConIn, &Key)) == EFI_NOT_READY);
    return Status; // Otherwise, wait for keystroke to continue.

}