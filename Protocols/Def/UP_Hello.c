#include "UP_Hello.h"

// Template for Hello private data structure.
HELLO_PRIVATE_DATA gHelloPrivateDataTemplate = {
  HELLO_PRIVATE_DATA_SIGNATURE,
  {
    EFI_HELLO_PROTOCOL_REVISION,
    Greeting
  },
  0
};

// Member function
EFI_STATUS
EFIAPI
Greeting(
  IN EFI_HELLO_PROTOCOL    *This,
  IN CHAR16                *String
  )
{
  DEBUG( (EFI_D_ERROR, "[Hello]Greeting function enter system.\n") );
  HELLO_PRIVATE_DATA *Private;
  Private = HELLO_PRIVATE_DATA_FROM_THIS(This);

  gST->ConOut->SetAttribute(gST->ConOut, EFI_GREEN );

  Print(L"Your input: %s\n", String);
  ++Private->TotalCallNum;
  Print(L"Total Calling: %d times\n", Private->TotalCallNum);

  gST->ConOut->SetAttribute(gST->ConOut, EFI_LIGHTGRAY);  // default color
  //gST->ConIn->Reset(gST->ConIn, FALSE);  // ConIn Reset
  return EFI_SUCCESS;
}

// Entry point
EFI_STATUS
EFIAPI
InitializeHello(
  IN EFI_HANDLE          ImageHandle,
  IN EFI_SYSTEM_TABLE    *SystemTable
  )
{
  EFI_STATUS Status = 0;
  HELLO_PRIVATE_DATA *Instance = NULL;

  Instance = AllocateCopyPool((UINTN)sizeof(HELLO_PRIVATE_DATA), &gHelloPrivateDataTemplate);
  if (Instance == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  // Status = gBS->InstallProtocolInterface(
  //           &ImageHandle,
  //           &gEfiHelloProtocolGuid,
  //           EFI_NATIVE_INTERFACE,
  //           &Instance->Hello
  //           );

  // More error checking
  // Install one or more different protocol on a handle by GUID
  // Install protocol by pairs of GUID and protocol interface, terminated with NULL
  Status = gBS->InstallMultipleProtocolInterfaces(
            &ImageHandle,
            &gEfiHelloProtocolGuid,
            &Instance->Hello,
            NULL
            );

  if (EFI_ERROR(Status)) {
    goto ErrorExit;
  }
  
ErrorExit:
  if (EFI_ERROR(Status)) {
    if (Instance != NULL) {
      DEBUG( (EFI_D_ERROR, "[Hello]Install protocol failed - %d.\n", Status) );
      FreePool(Instance);
    } else {
      DEBUG((EFI_D_ERROR, "[Hello][%s][%d]:Out of resource.\n",__FUNCTION__, __LINE__));
    }
  }
  
  ASSERT_EFI_ERROR (Status);

  return Status;
}
