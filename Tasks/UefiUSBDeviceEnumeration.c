#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiLib.h>
#include <Protocol/UsbIo.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DevicePathToText.h>
// #include <Protocol/Usb2HostController.h>

#define TITLE_COLOR     EFI_LIGHTGREEN
#define INFO_COLOR      EFI_WHITE
#define DEFAULT_COLOR   EFI_LIGHTGRAY
#define SET_COLOR(x)  gST->ConOut->SetAttribute(gST->ConOut, x)

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

VOID
PrintTitle(
  IN CHAR16* Title
  );

VOID
PrintUsbDevInfo(
  IN EFI_USB_IO_PROTOCOL   *UsbDevIo
  );

VOID
PrintUsbDevType(
  IN EFI_USB_IO_PROTOCOL   *UsbDevIo
  );

VOID
PrintUsbDevPath(
  IN EFI_DEVICE_PATH_TO_TEXT_PROTOCOL  *DevPath2Text,
  IN EFI_DEVICE_PATH_PROTOCOL          *UsbDevicePath
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
  EFI_USB_IO_PROTOCOL                   *UsbDevIo = 0;     

  // Find Protocol
  Status = gBS->LocateProtocol(
            &gEfiDevicePathToTextProtocolGuid,
            NULL,
            &DevPath2Text
            );

  // Find USB Devices Handles
  Status = gBS->LocateHandleBuffer(
            ByProtocol,
            &gEfiUsbIoProtocolGuid,
            NULL,
            &HandleCount,
            &ControllerHandles                        
            );

  if ( EFI_ERROR(Status) ) {
    Print(L"Find Protocol or Handles Wrong.\n");
    return Status;
  }

  // Find path protocol under specified handle then convert it to text.
  for ( HandleIndex = 0; HandleIndex < HandleCount; ++HandleIndex ) {
    Status = gBS->HandleProtocol(
              ControllerHandles[HandleIndex],
              &gEfiDevicePathProtocolGuid,
              (VOID**)&UsbDevicePath
              );

    if ( EFI_ERROR(Status) ) {
      Print(L"gEfiDevicePathProtocolGuid HandleProtocol Failed at Index: %d\n", HandleIndex);
      continue;
    }
    
    Status = gBS->HandleProtocol(
              ControllerHandles[HandleIndex],
              &gEfiUsbIoProtocolGuid,
              (VOID**)&UsbDevIo
              );

    if ( EFI_ERROR(Status) ) {
      Print(L"gEfiUsbIoProtocolGuid HandleProtocol Failed at Index: %d\n", HandleIndex);
      continue;
    }

    // Error!!!!
    // Status = gBS->HandleProtocol(
    //           ControllerHandles[HandleIndex],
    //           &gEfiUsb2HcProtocolGuid,
    //           &UsbDevHC
    //           );

    PrintTitle(L"USB Device Number: ");
    Print(L"%d\n", HandleIndex+1);
    PrintUsbDevInfo(UsbDevIo);
    PrintUsbDevType(UsbDevIo);
    PrintUsbDevPath(DevPath2Text, UsbDevicePath);
  }

  gBS->FreePool(ControllerHandles);
  return EFI_SUCCESS;
}

VOID
PrintUsbDevInfo(
  IN EFI_USB_IO_PROTOCOL   *UsbDevIo
  )
{
  EFI_STATUS                            Status;
  EFI_USB_DEVICE_DESCRIPTOR             UsbDevDescriptor;  // Use Structure not Pointer
  CHAR16                                *StrManufacture = 0;
  CHAR16                                *StrProduct = 0;
  CHAR16                                *StrSerialNumber = 0;
  UINT16                                *LangIDTable = 0;
  UINT16                                LangIDSize = 0;

  Status = UsbDevIo->UsbGetDeviceDescriptor(UsbDevIo, &UsbDevDescriptor);
  Status = UsbDevIo->UsbGetSupportedLanguages(UsbDevIo, &LangIDTable, &LangIDSize);

  // Print USB Device Infomation(MPS)
  // MPS := Manufacture Product Serial
  if ( LangIDSize == 0 ) {
    Print(L"Manufacture Product SerialNo: %02x, %02x, %02x\n", UsbDevDescriptor.StrManufacturer, UsbDevDescriptor.StrProduct, UsbDevDescriptor.StrSerialNumber);
  } else {
    Status = UsbDevIo->UsbGetStringDescriptor(UsbDevIo, LangIDTable[0], UsbDevDescriptor.StrManufacturer, &StrManufacture);
    if ( !EFI_ERROR(Status) ) {
      PrintTitle(L"Manufacture: ");
      Print(L"%s\n", StrManufacture);
      gBS->FreePool(StrManufacture);
    }

    Status = UsbDevIo->UsbGetStringDescriptor(UsbDevIo, LangIDTable[0], UsbDevDescriptor.StrProduct, &StrProduct);
    if ( !EFI_ERROR(Status) ) {
      PrintTitle(L"Product: ");
      Print(L"%s\n", StrProduct);
      gBS->FreePool(StrProduct);
    }
    
    Status = UsbDevIo->UsbGetStringDescriptor(UsbDevIo, LangIDTable[0], UsbDevDescriptor.StrSerialNumber, &StrSerialNumber);
    if ( !EFI_ERROR(Status) ) {
      PrintTitle(L"Serial Number: ");
      Print(L"%s\n", StrSerialNumber);
      gBS->FreePool(StrSerialNumber);
    }
  }

  SET_COLOR(DEFAULT_COLOR);
}

VOID
PrintUsbDevType(
  IN EFI_USB_IO_PROTOCOL   *UsbDevIo
  )
{
  EFI_STATUS                            Status;
  EFI_USB_INTERFACE_DESCRIPTOR          UsbInterfaceDescriptor;  // Use Structure not Pointer

  Status = UsbDevIo->UsbGetInterfaceDescriptor(UsbDevIo, &UsbInterfaceDescriptor);
  // USB 2.0 spec, Section 9.6.5
  // USB Class Code | USB-IF
  PrintTitle(L"Type: ");
  switch (UsbInterfaceDescriptor.InterfaceClass) {
  case 0x01:
    Print(L"Audio\n");
    break;
  
  case 0x02:
    Print(L"Communications and CDC Control\n");
    break;

  case 0x03:
    Print(L"HID\n");
    break;
    
  case 0x05:
    Print(L"Physical\n");
    break;
    
  case 0x06:
    Print(L"Image\n");
    break;
    
  case 0x07:
    Print(L"Printer\n");
    break;
    
  case 0x08:
    Print(L"Mass Storage\n");
    break;

  case 0x0A:
    Print(L"CDC Data\n");
    break;

  case 0x0B:
    Print(L"Smart Card\n");
    break;

  case 0x0D:
    Print(L"Content Security\n");
    break;

  case 0x0E:
    Print(L"Video\n");
    break;

  case 0x0F:
    Print(L"Personal Healthcare\n");
    break;

  case 0x10:
    Print(L"Audio/Video Device\n");
    break;

  case 0x12:
    Print(L"USB Type-C Bridge Class\n");
    break;

  case 0x3C:
    Print(L"I3C Device Class\n");
    break;

  case 0xDC:
    Print(L"Diagnostic Device\n");
    break;

  case 0xE0:
    Print(L"Wireless Controller\n");
    break;

  case 0xEF:
    Print(L"Miscellaneous\n");
    break;

  case 0xFE:
    Print(L"Application Specific\n");
    break;

  case 0xFF:
    Print(L"Vender Specific\n");
    break;

  default:
    Print(L"Unknow\n");
    break;
  }

  SET_COLOR(DEFAULT_COLOR);
}

VOID
PrintUsbDevPath(
  IN EFI_DEVICE_PATH_TO_TEXT_PROTOCOL  *DevPath2Text,
  IN EFI_DEVICE_PATH_PROTOCOL          *UsbDevicePath
  )
{
  CHAR16                                *StrText = 0;       
  // Print USB Device Path
  StrText = DevPath2Text->ConvertDevicePathToText(UsbDevicePath, TRUE, TRUE);
  PrintTitle(L"Path: ");
  Print(L"%s\n", StrText);
  if (StrText != NULL) {
    gBS->FreePool(StrText);
  }

  PrintTitle(L"Path Node(Type, SubType): ");
  WalkThroughDevicePath(UsbDevicePath, PrintDevicePathNode); 

  SET_COLOR(DEFAULT_COLOR);
}

VOID
PrintTitle(
  IN CHAR16* Title
  )
{
  SET_COLOR(TITLE_COLOR);
  Print(L"%s", Title);
  SET_COLOR(INFO_COLOR);
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
  while ( !IsDevicePathEnd(pDevPath) ) {
    CallBack(pDevPath);
    pDevPath = NextDevicePathNode(pDevPath);
  }
  Print(L"(%d %d)\n\n", pDevPath->Type, pDevPath->SubType);
  return pDevPath;
}
