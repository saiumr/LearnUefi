// Test UPhello (EFI_HELLO_PROTOCOL)
// Must "load UPhello.efi" firstly
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include "../Protocols/Dec/UP_Hello.h"

EFI_STATUS
UefiMain(
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_HELLO_PROTOCOL *HelloInterface = NULL;

  gBS->LocateProtocol(
        &gEfiHelloProtocolGuid,
        (VOID*)NULL,
        &HelloInterface
        );
  HelloInterface->Hello(HelloInterface, L"Hello");

  return EFI_SUCCESS;
}