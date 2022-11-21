#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/TimerLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/IoLib.h> 

#define CMOS_INDEX    0x70
#define CMOS_DATA     0x71 
 
INTN
EFIAPI
ShellAppMain (
  IN UINTN Argc,
  IN CHAR16 **Argv
  ) 
{
  EFI_STATUS Status;
  EFI_EVENT Events[2] = {0}; 
  UINTN EventIndex = 0;

  UINT8 second = 0;
  UINT8 minute = 0;
  UINT8 hour = 0;
  UINT8 weekday = 0;
  UINT8 date = 0;
  UINT8 month = 0;
  UINT8 year = 0;

  Events[0] = gST->ConIn->WaitForKey;
  Status = gBS->CreateEvent(EVT_TIMER  , TPL_CALLBACK, (EFI_EVENT_NOTIFY)NULL, (VOID*)NULL, &Events[1]);
  Status = gBS->SetTimer(Events[1],TimerPeriodic , 10 * 1000 * 1000);
  
  while (1) { 
    IoWrite8 (CMOS_INDEX, 0x00); 
    second = IoRead8 (CMOS_DATA);    
    IoWrite8 (CMOS_INDEX, 0x02); 
    minute = IoRead8 (CMOS_DATA);
    IoWrite8 (CMOS_INDEX, 0x04); 
    hour = IoRead8 (CMOS_DATA);
    IoWrite8 (CMOS_INDEX, 0x06); 
    weekday = IoRead8 (CMOS_DATA);
    IoWrite8 (CMOS_INDEX, 0x07); 
    date = IoRead8 (CMOS_DATA);
    IoWrite8 (CMOS_INDEX, 0x08); 
    month = IoRead8 (CMOS_DATA);
    IoWrite8 (CMOS_INDEX, 0x09); 
    year = IoRead8 (CMOS_DATA); 
    Print (L"%02x/%02x/%02x %02x %02x:%02x:%02x\n", year,month,date,weekday,hour,minute,second);

    Status = gBS->WaitForEvent (2, Events, &EventIndex);
    if(EFI_SUCCESS == Status) {
        if(EventIndex == 0){
            break;
        }else if(EventIndex == 1){
            gST->ConOut->ClearScreen(gST->ConOut);
            continue;
        }
    }
  }
  Status = gBS->CloseEvent(Events[1]);
 
  return Status;
	
}

