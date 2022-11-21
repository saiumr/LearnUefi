#include "ParserMain.h"
// #include <stdio.h>
// #include <stdlib.h>
// #include <Library/BmpSupportLib.h>
// #include <Protocol/GraphicsOutput.h>
// #include <Library/UefiBootServicesTableLib.h>

INTN
ShellAppMain (
  IN UINTN Argc, 
  IN CHAR16 **Argv
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  Status = GIFParserLogoBltFromFileOrBuffer(Argv[1], NULL);
//   EFI_EVENT Events[2]       = {0};    // 0: Events[0] Time; 1: End Input
//   UINTN                            EventIndex      = 0;
//   UINT8 *buffer;
//   UINTN size;
//   UINTN GopBltSize;
//   UINTN Height;
//   UINTN Width;
//   EFI_GRAPHICS_OUTPUT_PROTOCOL     *GraphicsOutput = NULL;
//   EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Bltbuffer = NULL;
//   gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, (VOID **)&GraphicsOutput);

//   FILE *fp = fopen(Argv[1], "rb");
//   fseek(fp, 0, SEEK_END);
//   size = ftell(fp);
//   rewind(fp);
//   buffer = (UINT8 *)malloc(sizeof(UINT8)*size);
//   fread((VOID *)buffer, size, 1, fp);
//   fclose(fp);

//   TranslateBmpToGopBlt(
//               (VOID *)buffer,
//               size,
//               &Bltbuffer,
//               &GopBltSize,
//               &Height,
//               &Width
//               );

//   Status = gBS->CreateEvent(EVT_TIMER, TPL_CALLBACK, (EFI_EVENT_NOTIFY)NULL, (VOID*)NULL, &Events[0]);
//   Status = gBS->SetTimer(Events[0], TimerPeriodic, 30 * 100 * 100);  // ms
//   Events[1] = gST->ConIn->WaitForKey;
  
//   while (1) {
//     gBS->WaitForEvent(2, Events, &EventIndex);
//     switch (EventIndex) {
//       case 0:
//       {
//         GraphicsOutput->Blt(
//                         GraphicsOutput,
//                         Bltbuffer,
//                         EfiBltBufferToVideo,
//                         0, 
//                         0, 
//                         (GraphicsOutput->Mode->Info->HorizontalResolution - Width) / 2, 
//                         (GraphicsOutput->Mode->Info->VerticalResolution - Height) / 2,
//                         Width,
//                         Height,
//                         Width * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
//                         );
//       }
//       break;

//       default:
//         goto end;
//         break;
//     }
//   }
  
// end:
//   free(buffer);
//   free(Bltbuffer);

  if (EFI_ERROR(Status)) {
    Print(L"- Failed -\n");
  }
  else {
    Print(L"- Done -\n");
  }

  return 0;
}
