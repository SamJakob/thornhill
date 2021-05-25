#include "logging.h"

EFI_STATUS THBPrintBanner(EFI_SYSTEM_TABLE* SystemTable, bool ShouldClearScreen) {

    EFI_STATUS Status;

    if (ShouldClearScreen) SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
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

    return Status;

}

EFI_STATUS THBPrintMessage(EFI_SYSTEM_TABLE* SystemTable, CHAR16* Message) {

    EFI_STATUS Status;
    EFI_TIME CurrentTime;

    Status = SystemTable->RuntimeServices->GetTime(&CurrentTime, NULL);
    UINT8 CurrentHour = CurrentTime.Hour;
    bool isPM = false;

    if (CurrentHour >= 12) {
        isPM = true;
        CurrentHour -= 12;
    }

    if (CurrentHour == 0)
        CurrentHour = 12;

    Print(
        (CHAR16*) L"[%d/%02d/%d %d:%02d:%02d %s] %s\r\n",
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

}