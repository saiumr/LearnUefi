/** @file
  This is a test for show bmp

**/

#include "HiiPic2.h"

/**
  load bmp file to buffer

  @param   IN      CHAR16     *FileName
  @param   IN OUT  UINT8      **FileDataBuffer
  @param   IN OUT  UINTN      *DataLength

  @retval  EFI_SUCCESS
  @retval            

**/
EFI_STATUS
EFIAPI
LoadFiletoBuffer (
  IN      CHAR16           *FileName,
  IN OUT  UINT8            **FileDataBuffer,
  IN OUT  UINT64            *DataLength
)
{
  EFI_STATUS                      Status;
  EFI_FILE_HANDLE                 ReadFileHandle;
  EFI_FILE_PROTOCOL               *FileRoot;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystem;
  EFI_FILE_INFO                   *ReadFileInfo = NULL;
  

  Status = gBS->LocateProtocol(
						      &gEfiSimpleFileSystemProtocolGuid, 
						      NULL,
					     	  (VOID **)&SimpleFileSystem
                  );
  if (EFI_ERROR(Status)) {
		  Print (L"not found gEfiSimpleFileSystemProtocolGuid!\n");
      return Status;	
	}
  
  Status = SimpleFileSystem->OpenVolume(SimpleFileSystem, &FileRoot);
  if (EFI_ERROR(Status)) {
		Print (L"OpenVolume is error !\n");
    return Status;	
	}

  Status = FileRoot->Open(
						           FileRoot, 
							         &ReadFileHandle, 
							         FileName,
							         EFI_FILE_MODE_READ , 
							         0 
                       );
  if (EFI_ERROR (Status)) {
    Print (L"open file root is error!\n");
    return Status;
  }
  
  ReadFileInfo = ShellGetFileInfo( (SHELL_FILE_HANDLE)ReadFileHandle);

  *DataLength = ReadFileInfo->FileSize;
  *FileDataBuffer = (UINT8 *)AllocateZeroPool (*DataLength);

  Status = ReadFileHandle->Read (
                             ReadFileHandle,
                             DataLength,
                             *FileDataBuffer
                             );
  if (EFI_ERROR (Status)) {
    Print (L"Read file is error!\n");
    return Status;
  }	
  
  ReadFileHandle->Close (ReadFileHandle);
  FileRoot->Close (FileRoot);

  Print (L"Load File is Ok!\n");

  return Status;
}


/**
  UEFI application entry point which has an interface similar to a
  standard C main function.

  The ShellCEntryLib library instance wrappers the actual UEFI application
  entry point and calls this ShellAppMain function.

  @param  Argc             Argument count
  @param  Argv             The parsed arguments

  @retval  0               The application exited normally.
  @retval  Other           An error occurred.

**/
INTN
EFIAPI
ShellAppMain (
  IN UINTN Argc,
  IN CHAR16 **Argv
  )
{
  EFI_STATUS                       Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL     *GraphicsOutput = NULL;
  UINT8                            *BmpDataBuffer;
  UINT64                           DataLength;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL    *EepromBitmap = NULL;
  UINTN                            GopBltSize;
  UINTN                            Height;
  UINTN                            Width;
  
  //Status = gST->ConOut->EnableCursor(gST->ConOut,FALSE);
  //Status = gST->ConOut->SetAttribute(gST->ConOut,EFI_WHITE|EFI_BACKGROUND_BLACK);
  //Status = gST->ConOut->ClearScreen(gST->ConOut);

  Status = gBS->LocateProtocol (
                  &gEfiGraphicsOutputProtocolGuid,
                  NULL,
                  (VOID **)&GraphicsOutput
                  );
  if (EFI_ERROR (Status)) {
    GraphicsOutput = NULL;
    Print (L"locate protocol gEfiGraphicsOutputProtocolGuid is error!\n");
    return Status;
  }
  
  Status = LoadFiletoBuffer (
             Argv[1],
             &BmpDataBuffer,
             &DataLength
             );
  if (EFI_ERROR (Status)) {
    Print (L"Load Bmp to Buffer is error!\n");
    return Status;
  }

  //
  // bmp to blt
  //
  Status = TranslateBmpToGopBlt (
             (VOID *)BmpDataBuffer,
             DataLength,
             &EepromBitmap,
             &GopBltSize,
             &Height,
             &Width
             );
  if (EFI_ERROR (Status)) {
    if (EepromBitmap != NULL) {
      FreePool (EepromBitmap);
    }
    Print (L"TranslateBmpToGopBlt is error!\n");
    return Status;
  }
  
  Status = GraphicsOutput->Blt (
                             GraphicsOutput,
                             EepromBitmap,
                             EfiBltBufferToVideo,
                             0,
                             0,
                             0x0,
                             0x0,
                             Width,
                             Height,
                             Width * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                             );
  if (EFI_ERROR (Status)) {
    Print (L"GraphicsOutput Blt is error!\n");
    return Status;
  }
  if (EepromBitmap != NULL)
    FreePool (EepromBitmap);

  //Status = gST->ConOut->EnableCursor(gST->ConOut,TRUE);
  return 0;
}

