#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

EFI_STATUS WaitForKeyWithTimer();

VOID MyEventNotify(IN EFI_EVENT event, IN VOID *context);
EFI_STATUS TestNotify();

EFI_STATUS My3sTimer();

VOID MyNotify(IN EFI_EVENT event, IN VOID *context);
EFI_STATUS TimerWithNotify();

EFI_STATUS
UefiMain(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE* SystemTable
    )
{
    EFI_STATUS Status;
    //Status = WaitForKeyWithTimer();
    //Status = TestNotify();
    //Status = My3sTimer();
    Status = TimerWithNotify();
    if (Status == EFI_SUCCESS) {
        Print(L"-- Done --\n");
    }
    else {
        Print(L"-- Failed --\n");
    }

    return EFI_SUCCESS;
}

EFI_STATUS TimerWithNotify() {
    EFI_STATUS status;
    EFI_EVENT my_event;
    INTN index;
    Print(L"EVT_TIMER | EVT_NOTIFY_SIGNAL\n");
    status = gBS->CreateEvent(EVT_TIMER | EVT_NOTIFY_SIGNAL, TPL_CALLBACK, (EFI_EVENT_NOTIFY)MyNotify, (VOID*)L"Time Out!", &my_event);
    status = gBS->SetTimer(my_event, TimerPeriodic, 30*1000*1000);
    status = gBS->SignalEvent(my_event);
    //status = gBS->WaitForEvent(1, &my_event, &index);
    if (EFI_SUCCESS == gBS->WaitForEvent(1, &(gST->ConIn->WaitForKey), &index)) {
        status = gBS->CloseEvent(my_event);
        return status;
    }
    return EFI_ABORTED;
}

VOID MyNotify(IN EFI_EVENT event, IN VOID *context) {
    static UINTN times = 0;
    Print(L"%s\n MyNotify signal %d\n", context, times);
    ++times;
}

EFI_STATUS My3sTimer() {
    EFI_EVENT threes_timer;
    EFI_STATUS status;
    UINTN index;
    status = gBS->CreateEvent(EVT_TIMER, TPL_CALLBACK, (EFI_EVENT_NOTIFY)NULL, (VOID*)NULL, &threes_timer);
    status = gBS->SetTimer(threes_timer, TimerPeriodic, 30*1000*1000);
    Print(L"Three seconds later automatically stop ( I'm in while(1) )\n");

    while (1) {
        // block operation
        gBS->WaitForEvent(1, &threes_timer, &index);
        
        // time up
        Print(L"Stop\n");
        break;
    }
    status = gBS->CloseEvent(threes_timer);
    return status;
}

EFI_STATUS TestNotify() {
    EFI_STATUS status;
    UINTN index = 0;
    EFI_EVENT my_event;
    status = gBS->CreateEvent(EVT_NOTIFY_WAIT, TPL_NOTIFY, (EFI_EVENT_NOTIFY)MyEventNotify, (VOID*)NULL, &my_event);
    status = gBS->WaitForEvent(1, &my_event, &index);
    status = gBS->CloseEvent(my_event);
    return status;
}

VOID MyEventNotify(IN EFI_EVENT event, IN VOID *context) {
    static UINTN times = 0;
    Print(L"MyEventNotify Wait %d\n", times);
    times += 1;
    if (times > 3) {
        gBS->SignalEvent(event);
    }
}

EFI_STATUS WaitForKeyWithTimer() {
    EFI_STATUS Status;
    UINTN EventIndex = 0;
    EFI_EVENT Events[2] = {0};
    // Set WaitForKey event
    Events[0] = gST->ConIn->WaitForKey;
    // Creat Timer event
    Status = gBS->CreateEvent(EVT_TIMER, TPL_CALLBACK, (EFI_EVENT_NOTIFY)NULL, (VOID*)NULL, &Events[1]);
    // Set Timer event attribute
    Status = gBS->SetTimer(Events[1], TimerPeriodic, 10*1000*1000);

    Status = gBS->WaitForEvent(2, Events, &EventIndex);
    if (Status == EFI_SUCCESS) {
        if (EventIndex == 0) {
            EFI_INPUT_KEY Key;
            Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
            Print(L"Key Event: %d, %c\n", Key.ScanCode, Key.UnicodeChar);
        }
        else if (EventIndex == 1) {
            Print(L"Key Event Timeout\n");
        }
        else {
            return EFI_LOAD_ERROR;
        }
    }
    return EFI_SUCCESS;
}
