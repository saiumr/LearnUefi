#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/HiiLib.h>
//#include <Library/ShellLib.h>
#include <Protocol/HiiString.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Guid/GlobalVariable.h>

//extern CHAR8 *exampleStrings;

EFI_STATUS
UefiMain (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
{
  EFI_STRING StrBuf = NULL;
  EFI_GUID mStrPackageGuid = {0x0ad75e9b, 0x0b11, 0x4eb1, 0xa7, 0xae, 0xf3, 0x6c, 0x42, 0x81, 0xd8, 0xc3};
  EFI_HANDLE HiiHandle = HiiAddPackages(&mStrPackageGuid, ImageHandle, HiiTestStrings, NULL);                // BASE_NAME + "Strings"
  //ShellPrintHiiEx(-1, -1, (CONST CHAR8*)"en-US", STRING_TOKEN (STR_LANGUAGE_SELECT), HiiHandle );          // print
  StrBuf = HiiGetString(HiiHandle, STRING_TOKEN(STR_LANGUAGE_SELECT), "en-US");                      // use lib, and another way is using protocol
  Print(L"%s\n", StrBuf);
  gBS->FreePool(StrBuf);
  StrBuf = NULL;

  EFI_STRING StrBufProtocol = NULL;
  UINTN      StrSize = 0;
  EFI_STATUS Status;
  EFI_HII_STRING_PROTOCOL *HiiStringOp;
  gBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL, &HiiStringOp);
  Status = HiiStringOp->GetString(HiiStringOp, "fr-FR", HiiHandle, STRING_TOKEN(STR_LANGUAGE_SELECT), StrBufProtocol, &StrSize, NULL);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    StrBufProtocol = AllocatePool(StrSize);
    Status = HiiStringOp->GetString(HiiStringOp, "fr-FR", HiiHandle, STRING_TOKEN(STR_LANGUAGE_SELECT), StrBufProtocol, &StrSize, NULL);
    Print(L"%s\n", StrBufProtocol);
    FreePool(StrBufProtocol);
    StrBufProtocol = NULL;
  }
  else {
    Print(L"GetString %r\n", Status);
  }

  CHAR8 *language;
  GetEfiGlobalVariable2(L"PlatformLangCodes", (VOID **)&language, &StrSize);
  Print(L"Support = %a, size = %d\n", language, StrSize);
  gBS->FreePool(language);
  language = NULL;

  GetEfiGlobalVariable2(L"PlatformLang", (VOID **)&language, &StrSize);
  Print(L"system lan = %a, size = %d\n", language, StrSize);
  gBS->FreePool(language);
  language = NULL;

  CHAR8 *DefaultLanguage = "zh-Hans";
  Status = gRT->SetVariable(L"PlatformLang", &gEfiGlobalVariableGuid, 
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS, 
                    AsciiStrSize(DefaultLanguage), 
                    DefaultLanguage);

  if (EFI_ERROR(Status)) {
      Print(L"error code = 0x%x\n", Status);
      return EFI_ABORTED;
  }

  StrBuf = HiiGetString(HiiHandle, STRING_TOKEN(STR_LANGUAGE_SELECT), "zh-Hans");   // system does not support zh-Hans, so here is blank.
  Print(L"%s\n", StrBuf);
  gBS->FreePool(StrBuf);
  StrBuf = NULL;

  gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW);
  EFI_GRAPHICS_OUTPUT_PROTOCOL            *GraphicsOutput;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION    *Info;
  UINTN                                   InfoSize;
  UINT32                                  Mode;
  UINTN                                   Index;
  EFI_INPUT_KEY                           Key;

  gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, &GraphicsOutput);

  for (UINT32 i = 0; i < GraphicsOutput->Mode->MaxMode; ++i) {
    Print(L"Mode %d\n", i);
    Status = GraphicsOutput->QueryMode(GraphicsOutput, i, &InfoSize, &Info);
    if (EFI_ERROR(Status) ) {
      Print(L"QueryMode %r\n", Status);
      break;
    }
    else {
      Print(L"Version %x, Width: %d, Height: %d, ScanLine: %d\n", Info->Version, Info->HorizontalResolution, Info->VerticalResolution, Info->PixelsPerScanLine);
    }
    FreePool(Info);
  }

  gBS->WaitForEvent(1, &(gST->ConIn->WaitForKey), &Index);
  gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
  Mode = (UINT32)StrDecimalToUintn(&Key.UnicodeChar);
  Print(L"Set Mode = %d\n", Mode);

  if (Mode < GraphicsOutput->Mode->MaxMode) {
    GraphicsOutput->SetMode(GraphicsOutput, Mode);
  }
  else {
    Print(L"Error.\n");
  }

  gST->ConOut->SetAttribute(gST->ConOut, EFI_LIGHTGRAY);
  return EFI_SUCCESS;
}