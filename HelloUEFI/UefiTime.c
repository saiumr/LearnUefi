#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

// Get Time and flash per 1 second.


EFI_STATUS
UefiMain(
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE    *SystemTable
)
{
    EFI_EVENT Event[2];
    UINTN Index;
    EFI_TIME Time;
    EFI_TIME_CAPABILITIES TimeCapability;
    gBS->CreateEvent(EVT_TIMER, TPL_CALLBACK, (EFI_EVENT_NOTIFY)NULL, (VOID*)NULL, &Event[0]);
    gBS->SetTimer(Event[0], TimerPeriodic, 10 * 1000 * 1000);  // 1s
    Event[1] = gST->ConIn->WaitForKey;

    gST->ConOut->SetAttribute(gST->ConOut, EFI_LIGHTCYAN);
    while (1) {
        SystemTable->RuntimeServices->GetTime(&Time, &TimeCapability);    
        gBS->WaitForEvent(2, Event, &Index);
        if (Index == 0) {
            gST->ConOut->ClearScreen(gST->ConOut);
            Print(L"%t:%02d\n", Time, Time.Second);
        }
        else {
            break;
        }
    }
    gBS->CloseEvent(Event[0]);
    gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE);

    Print(L"- Done -\n");
    return EFI_SUCCESS;
}