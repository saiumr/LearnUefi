#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>  // use gST access SystemTable

EFI_STATUS UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable) {
    EFI_STATUS Status;
    UINTN Index;
    EFI_INPUT_KEY Key;
    CHAR16 StrBuffer[4] = {0};
    gST->BootServices->WaitForEvent(1, &gST->ConIn->WaitForKey, &Index);
    Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
    StrBuffer[0] = Key.UnicodeChar;
    StrBuffer[1] = '\n';
    gST->ConOut->OutputString(gST->ConOut, StrBuffer);
    gST->ConOut->OutputString(gST->ConOut, L"Using gST accessed.\n");

    return EFI_SUCCESS;
}

/**
EFI_STATUS UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable) {
    EFI_STATUS Status;
    UINTN Index;
    EFI_INPUT_KEY Key;
    CHAR16 StrBuffer[3] = {0};
    SystemTable->BootServices->WaitForEvent(1, &SystemTable->ConIn->WaitForKey, &Index);
    Status = SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key);
    StrBuffer[0] = Key.UnicodeChar;
    StrBuffer[1] = '\n';
    SystemTable->ConOut->OutputString(SystemTable->ConOut, StrBuffer);

    return EFI_SUCCESS;
}
*/