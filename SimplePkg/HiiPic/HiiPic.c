#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/ShellLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BmpSupportLib.h>
#include <Library/DebugLib.h>

EFI_GRAPHICS_OUTPUT_PROTOCOL    *gGraphicsOut;

EFI_STATUS
EFIAPI
LocateGraphicsOut();

EFI_STATUS
EFIAPI
ShowAllModeInfo();

EFI_STATUS
EFIAPI
SetMode(
  IN UINT32 ModeNumber
);

EFI_STATUS
EFIAPI
DrawPic(
  IN CHAR16 *FileName
);


EFI_STATUS
UefiMain(
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
)
{
  // 1. Get EFI_GRAPHICS_OUTPUT_PROTOCOL instance.
  // 2. QueryMode get info, ensure resolution is reasonable.
  // 3. SetMode
  // 4. Use Blt() draw pic.
  EFI_STATUS Status;
  //Status = DrawPic(L"lm.bmp");
  Status = DrawPic(L"s1.bmp");
  if ( !EFI_ERROR (Status) ) {
    Print(L"- Done -\n");
  }
  else {
    Print(L"- Failed -\n");
  }

  return Status;
}


EFI_STATUS
EFIAPI
LocateGraphicsOut() 
{
  EFI_STATUS      Status;
  EFI_HANDLE      *GraphicsDevicesHandles;
  UINTN           HandleCount;
  UINTN           HandleIndex;

  Status = gBS->LocateHandleBuffer(
            ByProtocol,
            &gEfiGraphicsOutputProtocolGuid,
            NULL,
            &HandleCount,
            &GraphicsDevicesHandles
            );
  if ( EFI_ERROR(Status) ) {
    return Status;
  }

  for (HandleIndex = 0; HandleIndex < HandleCount; ++HandleIndex) {
    Status = gBS->HandleProtocol(
              GraphicsDevicesHandles[HandleIndex],
              &gEfiGraphicsOutputProtocolGuid,
              &gGraphicsOut
              );
    if ( EFI_ERROR(Status) ) {
      continue;
    } 
    else {
      return EFI_SUCCESS;
    }
  }

  return Status;
}

EFI_STATUS
EFIAPI
ShowAllModeInfo()
{
  EFI_STATUS                                Status;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION      *Info;
  UINT32                                    ModeIndex;
  UINTN                                     SizeOfInfo;

  for (ModeIndex = 0; ModeIndex < gGraphicsOut->Mode->MaxMode; ++ModeIndex) {
    Status = gGraphicsOut->QueryMode(gGraphicsOut, ModeIndex, &SizeOfInfo, &Info);
    if ( EFI_ERROR (Status) ) {
      return Status;
    }
    else {
      Print(L"Mode %d\n", ModeIndex);
      Print(L"Width = %d, Height = %d\n", Info->HorizontalResolution, Info->VerticalResolution);
    }
  }

  return Status;
}

EFI_STATUS
EFIAPI
SetMode(
  IN UINT32 ModeNumber
)
{
  EFI_STATUS Status;
  Status = gGraphicsOut->SetMode(gGraphicsOut, ModeNumber);

  return Status;
}

EFI_STATUS
EFIAPI
DrawPic(
  IN CHAR16 *FileName
)
{
  EFI_STATUS                          Status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL     *SimpleFileSystem = NULL;
  EFI_FILE_PROTOCOL                   *RootHandle = NULL, *FileHandle = NULL;   // EFI_FILE_PROTOCOL* == EFI_FILE_HANDLE
  EFI_FILE_INFO                       *FileInfo = NULL;

  UINT8                               *DataBuffer = NULL;
  UINTN                               DataLength;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL       *GopBlt = NULL;
  UINTN                               BltSize, Width, Height;

  
  Status = gBS->LocateProtocol(
            &gEfiSimpleFileSystemProtocolGuid,
            NULL,
            &SimpleFileSystem
            );
  if ( !EFI_ERROR (Status) ) {
    Status = SimpleFileSystem->OpenVolume(SimpleFileSystem, &RootHandle);
  }
  else {
    Print(L"gBS->LocateProtocol Failed.\n");
    return Status;
  }

  if ( !EFI_ERROR (Status) ) {
    Status = RootHandle->Open(RootHandle, &FileHandle, FileName, EFI_FILE_MODE_READ, 0);
  }
  else {
    Print(L"SimpleFileSystem->OpenVolume Failed.\n");
    return Status;
  }

  if ( !EFI_ERROR (Status) ) {
    FileInfo = ShellGetFileInfo(FileHandle);
    DataLength = FileInfo->FileSize;
    DataBuffer = (UINT8 *)AllocateZeroPool(DataLength);
    Status = FileHandle->Read(FileHandle, &DataLength, DataBuffer);
  }
  else {
    Print(L"RootHandle->Open Failed.\n");
    return Status;
  }


  // Start
  if ( !EFI_ERROR (Status) ) {
    Status = LocateGraphicsOut();
  }
  else {
    Print(L"FileHandle->Read Failed.\n");
    return Status;
  }
  
  if ( !EFI_ERROR (Status) ) {
    SetMode(gGraphicsOut->Mode->MaxMode-1);    // Max Resolution
    Status = ShowAllModeInfo();
  }
  else {
    Print(L"LocateGraphicsOut Failed.\n");
    return Status;
  }

  if ( !EFI_ERROR (Status) ) {
    Print(L"Here Here Here!\n");
    Status = TranslateBmpToGopBlt((VOID *)DataBuffer, DataLength, &GopBlt, &BltSize, &Height, &Width);
    Print(L"I'm Here!\n");
  }
  else {
    Print(L"ShowAllModeInfo Failed.\n");
    return Status;
  }

  if ( !EFI_ERROR (Status) ) {
    Status = gGraphicsOut->Blt(gGraphicsOut, GopBlt, EfiBltBufferToVideo, 
                            0, 0, 
                            0, 0, 
                            Width, Height, 
                            Width*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                            );
  }
  else {
    Print(L"TranslateBmpToGopBlt Failed.\n");
    return Status;
  }

  if ( !EFI_ERROR (Status) ) {
    Status = gST->ConOut->EnableCursor(gST->ConOut, TRUE);
  }
  else {
    Print(L"gGraphicsOut->Blt Failed.\n");
  }

  if ( !EFI_ERROR (Status) ) {
    FileHandle->Close(FileHandle);
    RootHandle->Close(RootHandle);
    FreePool(GopBlt);
    FreePool(DataBuffer);
  }

  return Status;
}
