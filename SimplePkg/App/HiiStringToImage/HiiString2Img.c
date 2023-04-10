#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/HiiLib.h>
#include <Protocol/HiiString.h>
#include <Protocol/SimpleTextOut.h>
#include <Library/BmpSupportLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Guid/GlobalVariable.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/HiiFont.h>
#include <Library/UefiHiiServicesLib.h>

EFI_STATUS
UefiMain (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
{
  EFI_HII_FONT_PROTOCOL *HiiFont;
  EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOut = NULL;
  EFI_STATUS            Status;
  EFI_IMAGE_OUTPUT  ImageOut;  
  EFI_IMAGE_OUTPUT *Image = &ImageOut;
  CHAR16 *Text = L"Hello, World!";
  EFI_FONT_DISPLAY_INFO FontDisplayInfo = {
    {0,    0,    0xFF, 0},    // BGRA FgColor
    {0xFF, 0xFF, 0xFF, 0},    // BGRA BgColor
    EFI_FONT_INFO_ANY_FONT,
    {EFI_HII_FONT_STYLE_NORMAL, 12, L'0'}  // FontName is a pointer, it points to Char16 string. use CopyMem()
  };

  Status = gBS->LocateProtocol(&gEfiHiiFontProtocolGuid, NULL, (VOID **)&HiiFont);
  Status = gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, (VOID **)&GraphicsOut);
  if (EFI_ERROR(Status)) {
    Print(L"Failed to locate HiiFontProtocol: %r\n", Status);
    return Status;
  }

  ImageOut.Width = (UINT16)GraphicsOut->Mode->Info->HorizontalResolution;
  ImageOut.Height = (UINT16)GraphicsOut->Mode->Info->VerticalResolution;
  ImageOut.Image.Screen = GraphicsOut;

  // Status=gST->ConOut->EnableCursor (gST->ConOut, TRUE);

  Status = HiiFont->StringToImage(
    HiiFont,
    EFI_HII_IGNORE_IF_NO_GLYPH | EFI_HII_OUT_FLAG_CLIP |
    EFI_HII_OUT_FLAG_CLIP_CLEAN_X | EFI_HII_OUT_FLAG_CLIP_CLEAN_Y |   
    EFI_HII_IGNORE_LINE_BREAK |  EFI_HII_DIRECT_TO_SCREEN,  
    Text,
    &FontDisplayInfo,
    &Image,
    100,
    100,
    NULL,
    NULL,
    NULL
  );

  // Status=gST->ConOut->EnableCursor (gST->ConOut, FALSE);

  if (EFI_ERROR(Status)) {
      Print(L"Failed to convert text to image: %r\n", Status);
      return Status;
  }

  return EFI_SUCCESS;
}