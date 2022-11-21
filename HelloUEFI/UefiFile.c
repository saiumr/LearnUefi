/**@UefiFile.c
   (Use events) open(create file if no specified file), write, read, flush, delete file
   In UEFI file system, EFI_FILE_HANDLE is a pointer to EFI_FILE_PROTOCOL. 
   And there are some asynchronous operations(OpenEx..) of file io.(page 163)
 **/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/SimpleFileSystem.h>

EFI_STATUS
UefiMain(
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS Status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystem = 0;
  EFI_FILE_PROTOCOL *Root = 0;
  Status = gBS->LocateProtocol(
            &gEfiSimpleFileSystemProtocolGuid,
            (VOID*)NULL,
            (VOID**)&SimpleFileSystem
            );
  if (EFI_ERROR (Status)) {
    Print(L"Can't found simple file system\n");
    return Status;
  }

  Status = SimpleFileSystem->OpenVolume(SimpleFileSystem, &Root);

  // create directory
  EFI_FILE_PROTOCOL *EfiDirectory = 0;
  Status = Root->Open(
            Root,
            &EfiDirectory,
            (CHAR16*)L"zombie",
            EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
            EFI_FILE_DIRECTORY
            );
  if (!EFI_ERROR (Status)) {
    Print(L"Create directory \"zombie\" under Root\n");
  }
  
  // create a file
  EFI_FILE_PROTOCOL *Readme = 0;
  Status = EfiDirectory->Open(
            EfiDirectory,
            &Readme,
            (CHAR16*)L"readme.txt",
            EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
            0
            );
  if (!EFI_ERROR (Status)) {
    Print(L"Create file \"readme.txt\" under \"zombie\"\n");
  }

  // write
  UINTN WBufferSize;
  CHAR16 *WText = L"This is readme text file, test file operations";
  WBufferSize = StrLen(WText) * 2 + 2;
  Status = Readme->Write(
            Readme,
            &WBufferSize,
            WText
            );
  if (!EFI_ERROR (Status)) {
    Print(L"Write %d bytes in \"readme.txt\"\n", WBufferSize);
  }

  // !Attention: constantly write/read must watch out position of file pointer
  Status = Readme->SetPosition(Readme, 0);
  // Read
  UINTN RBufferSize = 128;
  CHAR16 RBuffer[129];
  Status = Readme->Read(
            Readme,
            &RBufferSize,
            RBuffer
            );
  if (!EFI_ERROR (Status)) {
    RBuffer[RBufferSize] = 0;
    Print(L"Text %d bytes: %s\n", RBufferSize, RBuffer);
  }

  Readme->Close(Readme);
  Root->Close(Root);

  return EFI_SUCCESS;
}