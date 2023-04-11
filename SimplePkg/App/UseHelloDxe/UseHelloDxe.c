#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>

#include <Protocol/HelloDxe.h>

EFI_STATUS
UefiMain (
  IN EFI_HANDLE          ImageHandle,
  IN EFI_SYSTEM_TABLE    *SystemTable
  )
{
  EFI_STATUS               Status;
  EFI_HELLO_DXE_PROTOCOL   *HelloDxe = NULL;

  DEBUG((EFI_D_ERROR, "USE MY PROTOCOL!\n"));  // No Output
  DEBUG((EFI_D_ERROR, "USE MY PROTOCOL!\n"));
  DEBUG((EFI_D_INFO, "USE MY PROTOCOL!\n"));

  Status = gBS->LocateProtocol(&gEfiHelloDxeProtocolGuid,
                      NULL,
                      (VOID **)&HelloDxe
                      );

  if (EFI_ERROR(Status)) {
    Print(L"!ERROR Status: %r\n", Status);
    return Status;
  }

  Print(L"Here is customized protocol: \n");
  HelloDxe->SayHello(HelloDxe);

  return Status;
}

