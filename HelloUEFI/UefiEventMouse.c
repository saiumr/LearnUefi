#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/SimplePointer.h>

EFI_STATUS
EFIAPI
TestMouseSimple();

EFI_STATUS
UefiMain(
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  TestMouseSimple();
  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
TestMouseSimple()
{
  EFI_STATUS Status;
  EFI_SIMPLE_POINTER_PROTOCOL *Mouse = 0;
  EFI_SIMPLE_POINTER_STATE State;
  EFI_EVENT Events[2];

  // Display cursor
  gST->ConOut->EnableCursor(gST->ConOut, TRUE);

  // Find mouse device
  Status = gBS->LocateProtocol(
              &gEfiSimplePointerProtocolGuid,
              NULL,
              (VOID**)&Mouse
              );

  // Reset mouse
  Mouse->Reset(Mouse, TRUE);

  Events[0] = Mouse->WaitForInput;
  Events[1] = gST->ConIn->WaitForKey;

  while (1) {
    EFI_INPUT_KEY Key;
    UINTN Index;
    Status = gBS->WaitForEvent(2, Events, &Index);
    if (Index == 0) {
      Status = Mouse->GetState(Mouse, &State);
      Print(L"X:%d Y:%d Z:%d L:%d R:%d\n", 
        State.RelativeMovementX,
        State.RelativeMovementY,
        State.RelativeMovementZ,
        State.LeftButton,
        State.RightButton
        );
    }
    else {
      Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
      if (Key.UnicodeChar == 'q') {
        break;
      }
    }
  }

  return EFI_SUCCESS;
}
