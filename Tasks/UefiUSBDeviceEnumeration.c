#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiLib.h>
#include <Protocol/UsbIo.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DevicePathToText.h>

EFI_STATUS
EFIAPI
PrintDevicePathNode(
  IN EFI_DEVICE_PATH_PROTOCOL *Node
  );

EFI_DEVICE_PATH_PROTOCOL*
EFIAPI
WalkThroughDevicePath(
  IN EFI_DEVICE_PATH_PROTOCOL *DevPath,
  IN EFI_STATUS (EFIAPI *CallBack)(IN EFI_DEVICE_PATH_PROTOCOL*)
  );

EFI_STATUS
EFIAPI
EnumerateUSBDevices();

EFI_STATUS
UefiMain(
  IN EFI_HANDLE          ImageHandle,
  IN EFI_SYSTEM_TABLE    *SystemTable
  )
{
  EFI_STATUS Status;

  Status = EnumerateUSBDevices();

  if ( EFI_ERROR(Status) ) {
    Print(L"- Failed -\n");
    return Status;
  } else {
    Print(L"- Done -\n");
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EnumerateUSBDevices()
{
  EFI_STATUS                            Status;
  UINTN                                 HandleIndex, HandleCount;
  EFI_HANDLE                            *ControllerHandles = 0;
  EFI_DEVICE_PATH_TO_TEXT_PROTOCOL      *DevPath2Text = 0;
  EFI_DEVICE_PATH_PROTOCOL              *UsbDevicePath = 0;
  EFI_USB_DEVICE_DESCRIPTOR             *UsbDevDescriptor = 0;
  EFI_USB_IO_PROTOCOL                   *UsbDev = 0;     
  CHAR16                                *StrText = 0;       

  // Find Protocol
  Status = gBS->LocateProtocol(
            &gEfiDevicePathToTextProtocolGuid,
            (VOID*)NULL,
            (VOID**)&DevPath2Text
            );

  // Find USB Devices Handles
  Status = gBS->LocateHandleBuffer(
            ByProtocol,
            &gEfiUsbIoProtocolGuid,
            (VOID*)NULL,
            &HandleCount,
            &ControllerHandles                        
            );

  if ( EFI_ERROR(Status) ) {
    Print(L"Find Protocol or Handles Wrong.\n");
    return Status;
  }

  // Find path protocol under specified handle and convert it to text.
  for ( HandleIndex = 0; HandleIndex < HandleCount; ++HandleIndex ) {
    Status = gBS->HandleProtocol(
              ControllerHandles[HandleIndex],
              &gEfiDevicePathProtocolGuid,
              &UsbDevicePath
              );

    if ( EFI_ERROR(Status) ) {
      continue;
    }
    
    Status = gBS->HandleProtocol(
              ControllerHandles[HandleIndex],
              &gEfiUsbIoProtocolGuid,
              &UsbDev
              );

    Status = UsbDev->UsbGetDeviceDescriptor(UsbDev, UsbDevDescriptor);

    // Print USB Device Infomation()
    gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE);
    Print(L"StrManufacture: %02x\n", UsbDevDescriptor->StrManufacturer);
    Print(L"StrProduct: %02x\n", UsbDevDescriptor->StrProduct);
    Print(L"StrSerialNumber: %02x\n", UsbDevDescriptor->StrSerialNumber);

    // Print USB Device Path
    StrText = DevPath2Text->ConvertDevicePathToText(UsbDevicePath, TRUE, TRUE);
    gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW);
    Print(L"%s\n", StrText);
    if (StrText != NULL) {
      gBS->FreePool(StrText);
    }

    WalkThroughDevicePath(UsbDevicePath, PrintDevicePathNode); 
  }

  gST->ConOut->SetAttribute(gST->ConOut, EFI_LIGHTGRAY);
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
PrintDevicePathNode(
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
  gST->ConOut->SetAttribute(gST->ConOut, EFI_GREEN);
  while ( !IsDevicePathEnd(pDevPath) ) {
    CallBack(pDevPath);
    pDevPath = NextDevicePathNode(pDevPath);
  }
  Print(L"(%d %d)\n\n", pDevPath->Type, pDevPath->SubType);
  return pDevPath;
}
