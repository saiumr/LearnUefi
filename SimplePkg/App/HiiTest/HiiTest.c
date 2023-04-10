#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/HiiLib.h>
//#include <Library/ShellLib.h>
#include <Protocol/HiiString.h>
#include <Protocol/SimpleTextOut.h>
#include <Library/BmpSupportLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Guid/GlobalVariable.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/HiiFont.h>
#include <Library/UefiHiiServicesLib.h>

//extern CHAR8 *exampleStrings;

EFI_STATUS
UefiMain (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
{
  EFI_STRING StrBuf = NULL;
  EFI_GUID mStrPackageGuid = {0x0ad75e9b, 0x0b11, 0x4eb1, 0xa7, 0xae, 0xf3, 0x6c, 0x42, 0x81, 0xd8, 0xc3};
  EFI_HANDLE HiiHandle = HiiAddPackages(&mStrPackageGuid, ImageHandle, HiiTestStrings, NULL);                // The last parameter must be NULL, Gen BASE_NAME + "Strings" string assets
  //ShellPrintHiiEx(-1, -1, (CONST CHAR8*)"en-US", STRING_TOKEN (STR_LANGUAGE_SELECT), HiiHandle );          // print
  StrBuf = HiiGetString(HiiHandle, STRING_TOKEN(STR_LANGUAGE_SELECT), "en-US");                      // use lib, and another way is using protocol
  Print(L"%s\n", StrBuf);
  gBS->FreePool(StrBuf);   // use lib just free
  StrBuf = NULL;

  EFI_STRING ProtocolOpString = NULL;
  UINTN      StrSize = 0;
  UINT16 StrID = 0;
  EFI_STRING NewStr = L"New String";
  EFI_STATUS Status;
  EFI_HII_STRING_PROTOCOL *HiiStringOp;
  gBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL, &HiiStringOp);
  Status = HiiStringOp->NewString(HiiStringOp, HiiHandle, &StrID, "en-US", L"English", NewStr, NULL);
  Print(L"NewString: %d\n", StrID);
  Status = HiiStringOp->GetString(HiiStringOp, "fr-FR", HiiHandle, STRING_TOKEN(STR_LANGUAGE_SELECT), ProtocolOpString, &StrSize, NULL);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    ProtocolOpString = AllocatePool(StrSize);  // use protocol needs allocate
    Status = HiiStringOp->GetString(HiiStringOp, "fr-FR", HiiHandle, STRING_TOKEN(STR_LANGUAGE_SELECT), ProtocolOpString, &StrSize, NULL);
    Print(L"GetString: %s\n", ProtocolOpString);
    FreePool(ProtocolOpString);
    ProtocolOpString = NULL;
  }
  else {
    Print(L"GetString %r\n", Status);
  }

  CHAR8 *language = NULL;
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

  // EFI_GRAPHICS_OUTPUT_PROTOCOL
  gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW);
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL         *SimpleTextOut = NULL;
  EFI_GRAPHICS_OUTPUT_PROTOCOL            *GraphicsOutput = NULL;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION    *Info = NULL;
  UINTN                                   InfoSize = 0;
  UINT32                                  Mode = 0;
  UINTN                                   Index = 0;
  EFI_INPUT_KEY                           Key;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL           BltBuffer[1] = {144, 233, 181, 127};  // Green: B, G, R, reserved
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL           *BltBuffer2bmp = NULL;
  VOID                                    *BmpBuffer = NULL;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL         *SimpleFile = NULL;
  EFI_HII_FONT_PROTOCOL                   *HiiFont = NULL;

  gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, &GraphicsOutput);
  gBS->LocateProtocol(&gEfiSimpleTextOutProtocolGuid, NULL, &SimpleTextOut);
  gBS->LocateProtocol(&gEfiSimpleFileSystemProtocolGuid, NULL, &SimpleFile);
  gBS->LocateProtocol(&gEfiHiiFontProtocolGuid, NULL, &HiiFont);

  for (UINT32 i = 0; i < GraphicsOutput->Mode->MaxMode; ++i) {
    Print(L"Mode %d\n", i);
    Status = GraphicsOutput->QueryMode(GraphicsOutput, i, &InfoSize, &Info);
    if (EFI_ERROR(Status) ) {
      Print(L"QueryMode %r\n", Status);
      break;
    }
    else {
      Print(L"CurrentMode: %u, Version %x, Width: %d, Height: %d, ScanLine: %d\n", GraphicsOutput->Mode->Mode, Info->Version, Info->HorizontalResolution, Info->VerticalResolution, Info->PixelsPerScanLine);
    }

    Print(L"PixelFormat: ");
    switch (Info->PixelFormat) {
      case PixelRedGreenBlueReserved8BitPerColor:
        Print(L"PixelRedGreenBlueReserved8BitPerColor\n");
        break;
      case PixelBlueGreenRedReserved8BitPerColor:
        Print(L"PixelBlueGreenRedReserved8BitPerColor\n");
        break;
      case PixelBitMask:
        Print(L"PixelBitMask\n");
        break;
      case PixelBltOnly:
        Print(L"PixelBltOnly\n");
        break;
      case PixelFormatMax:
        Print(L"PixelFormatMax no more pixel format\n");
        break;
      default:
        break;
    }
  }

  Print(L"Press 'a', 'b', 'c' or select function, then ouput a string by string2image\n");
  gBS->WaitForEvent(1, &(gST->ConIn->WaitForKey), &Index);
  gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
  
  // presenting green screen
  if (Key.UnicodeChar == L'a') {
    Status = GraphicsOutput->Blt(
              GraphicsOutput,
              BltBuffer,
              EfiBltVideoFill,
              0, 0,
              0, 0,
              GraphicsOutput->Mode->Info->HorizontalResolution, GraphicsOutput->Mode->Info->VerticalResolution,
              0
            );
    if ( EFI_ERROR(Status) ) {
      Print(L"Blt: %r\n", Status);
    }
  }
  else if (Key.UnicodeChar == L'b') {
    Status = GraphicsOutput->Blt(
      GraphicsOutput,
      0,
      EfiBltVideoToVideo,
      100, 100,
      0, 0,
      50, 50,
      0
    );
    if ( EFI_ERROR(Status) ) {
      Print(L"Blt: %r\n", Status);
    }
  }
  else if (Key.UnicodeChar == L'c') {
    // GopBltBuffer to BMP (ScreenShot)
    UINT32 BufferSize = GraphicsOutput->Mode->Info->HorizontalResolution * GraphicsOutput->Mode->Info->VerticalResolution * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
    BltBuffer2bmp = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL*)AllocatePool(BufferSize);
    Status = GraphicsOutput->Blt(
      GraphicsOutput,
      BltBuffer2bmp,
      EfiBltVideoToBltBuffer,
      0, 0,
      0, 0,
      GraphicsOutput->Mode->Info->HorizontalResolution, GraphicsOutput->Mode->Info->VerticalResolution,
      GraphicsOutput->Mode->Info->HorizontalResolution * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
    );
    if ( EFI_ERROR(Status) ) {
      Print(L"Blt: %r\n", Status);
    }

    Status = TranslateGopBltToBmp(BltBuffer2bmp, 
                                  GraphicsOutput->Mode->Info->VerticalResolution, GraphicsOutput->Mode->Info->HorizontalResolution, 
                                  &BmpBuffer, &BufferSize
                                  );
    if ( EFI_ERROR(Status) ) {
      Print(L"Blt Transfer: %r\n", Status);
    }

    EFI_FILE_PROTOCOL *Root = NULL;
    Status = SimpleFile->OpenVolume(SimpleFile, &Root);
    if ( EFI_ERROR(Status) ) {
      Print(L"SimpleFile Open Root: %r\n", Status);
    }
    EFI_FILE_PROTOCOL *BmpHandle = NULL;
    Status = Root->Open(Root, &BmpHandle, L"ScreenShot.bmp", 
                        EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 0
                        );
    if ( EFI_ERROR(Status) ) {
      Print(L"SimpleFile Open BmpHandle: %r\n", Status);
    }
    Status = BmpHandle->Write(BmpHandle, (UINTN*)&BufferSize, BmpBuffer);
    if ( EFI_ERROR(Status) ) {
      Print(L"SimpleFile Write BmpHandle: %r\n", Status);
    }
  }
  else {
    Mode = (UINT32)StrDecimalToUintn(&Key.UnicodeChar);
    Print(L"Set Mode = %d\n", Mode);

    if (Mode < GraphicsOutput->Mode->MaxMode) {
      GraphicsOutput->SetMode(GraphicsOutput, Mode);
    }
    else {
      Print(L"Error.\n");
    }
  }

  // EFI_FONT_PROTOCOL: Output string by BMP picture
  CHAR16 *OutBmpString = L"Hello Bmp String!";
  CONST EFI_FONT_DISPLAY_INFO FontDisplayInfo = {
    {0,    0xFF,    0, 0},    // BGRA FgColor
    {0xFF, 0xFF, 0xFF, 0},    // BGRA BgColor
    EFI_FONT_INFO_ANY_FONT,
    {EFI_HII_FONT_STYLE_NORMAL, 0, L'0'}
  };
  
  EFI_IMAGE_OUTPUT ImageOut = {800, 600, NULL};
  ImageOut.Image.Bitmap = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL*)AllocatePool(GraphicsOutput->Mode->Info->HorizontalResolution*GraphicsOutput->Mode->Info->VerticalResolution*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  
  EFI_IMAGE_OUTPUT Screen = {800, 600, NULL};
  Screen.Image.Screen = GraphicsOutput;
  EFI_IMAGE_OUTPUT *ScreenPtr = &Screen;
  
  // Status = HiiFont->StringToImage(HiiFont,
  //                        EFI_HII_IGNORE_IF_NO_GLYPH | EFI_HII_IGNORE_LINE_BREAK | \
  //                        EFI_HII_OUT_FLAG_CLIP | EFI_HII_OUT_FLAG_CLIP_CLEAN_X | EFI_HII_OUT_FLAG_CLIP_CLEAN_Y | \
  //                        EFI_HII_OUT_FLAG_TRANSPARENT | EFI_HII_DIRECT_TO_SCREEN,
  //                        OutBmpString,
  //                        &FontDisplayInfo,
  //                        (EFI_IMAGE_OUTPUT**)(&ImageOut),
  //                        GraphicsOutput->Mode->Info->HorizontalResolution-200, 100,
  //                        NULL, NULL, NULL
  //                        );
  // if ( EFI_ERROR(Status) ) {
  //   Print(L"HiiFont StringToImage ImageOut: %r\n", Status);
  // }
  
  Status = HiiFont->StringToImage(HiiFont,
                         EFI_HII_IGNORE_IF_NO_GLYPH | EFI_HII_IGNORE_LINE_BREAK | \
                         EFI_HII_OUT_FLAG_CLIP | EFI_HII_OUT_FLAG_CLIP_CLEAN_X | EFI_HII_OUT_FLAG_CLIP_CLEAN_Y | \
                         EFI_HII_OUT_FLAG_TRANSPARENT | EFI_HII_DIRECT_TO_SCREEN,
                         OutBmpString,
                         &FontDisplayInfo,
                         &ScreenPtr,
                        100,100,
                         NULL, NULL, NULL
                         );
  if ( EFI_ERROR(Status) ) {
      Print(L"HiiFont StringToImage Screen: %r\n", Status);
  }
  FreePool(ImageOut.Image.Bitmap);

  FreePool(Info);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_LIGHTGRAY);
  Print(L"- Done -\n");
  return EFI_SUCCESS;
}