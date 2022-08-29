/** @UefiDevecePath.c
    This file completes function of displaying disk io device path. Device path consistent of path nodes, and 
    the most important params are Type and Subtype, they indicate what node it is.
 **/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DevicePathToText.h>
#include <Protocol/DiskIo.h>

EFI_STATUS
EFIAPI
PrintNode(
  IN EFI_DEVICE_PATH_PROTOCOL *Node
  );

EFI_DEVICE_PATH_PROTOCOL*
EFIAPI
WalkThroughDevicePath(
  IN EFI_DEVICE_PATH_PROTOCOL *DevPath,
  IN EFI_STATUS (EFIAPI *CallBack)(IN EFI_DEVICE_PATH_PROTOCOL*)
  );

EFI_STATUS
UefiMain(
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS Status;
  UINTN HandleIndex, NumHandles;
  EFI_HANDLE *ControllerHandle = 0;
  EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *Device2TextProtocol = 0;
  
  // Find protocol
  Status = gBS->LocateProtocol(
            &gEfiDevicePathToTextProtocolGuid,
            (VOID*)NULL,
            (VOID**)&Device2TextProtocol
            );
  
  // Find disk devices handles
  Status = gBS->LocateHandleBuffer(
            ByProtocol,
            &gEfiDiskIoProtocolGuid,
            (VOID*)NULL,
            &NumHandles,
            &ControllerHandle
            );
  
  if (EFI_ERROR(Status)) {
    Print(L"Locate Failed\n");
    return Status;
  }

  // Find path protocol under specified handle and convert it to text
  for (HandleIndex = 0; HandleIndex < NumHandles; ++HandleIndex) {
    EFI_DEVICE_PATH_PROTOCOL *DiskDevicePath = 0;
    Status = gBS->OpenProtocol(
              ControllerHandle[HandleIndex],
              &gEfiDevicePathProtocolGuid,
              (VOID**)&DiskDevicePath,
              gImageHandle,
              NULL,
              EFI_OPEN_PROTOCOL_GET_PROTOCOL
              );
    
    if (EFI_ERROR(Status)) {
      continue;
    }

    {
      CHAR16 *TextDevicePath = 0;
      TextDevicePath = Device2TextProtocol->ConvertDevicePathToText(DiskDevicePath, TRUE, TRUE);
      Print(L"%s\n", TextDevicePath);
      if (TextDevicePath) {
        gBS->FreePool(TextDevicePath);
      }
    }
    
    WalkThroughDevicePath(DiskDevicePath, PrintNode);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
PrintNode(
  IN EFI_DEVICE_PATH_PROTOCOL *Node
  )
{
  Print(L"(%d %d)/", Node->Type, Node->SubType);
  return EFI_SUCCESS;
}

EFI_DEVICE_PATH_PROTOCOL*
EFIAPI
WalkThroughDevicePath(
  IN EFI_DEVICE_PATH_PROTOCOL *DevPath,
  IN EFI_STATUS (EFIAPI *CallBack)(IN EFI_DEVICE_PATH_PROTOCOL*)
  )
{
  EFI_DEVICE_PATH_PROTOCOL *pDevPath = DevPath;
  while ( !IsDevicePathEnd(pDevPath) ) {
    CallBack(pDevPath);
    pDevPath = NextDevicePathNode(pDevPath);
  }
  Print(L"(%d %d)\n\n", pDevPath->Type, pDevPath->SubType);
  return pDevPath;
}
