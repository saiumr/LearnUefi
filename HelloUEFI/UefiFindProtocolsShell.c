#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

INTN 
ShellAppMain (
  IN UINTN Argc, 
  IN CHAR16 **Argv
  ) 
{
    EFI_HANDLE *controller_handles = NULL;
    UINTN handles_num, handle_count;

    // get all of handle in an array
    gBS->LocateHandleBuffer(AllHandles, NULL, NULL, &handles_num, &controller_handles);
    for (handle_count = 0; handle_count < handles_num; ++handle_count) {
        Print(L"Handles[%d]: 0x%08x\n", handle_count, controller_handles[handle_count]);
    }

    Print(L"Handles[0]: 0x%08x\n", controller_handles[0]);

    EFI_GUID **guid_buffer = NULL;
    UINTN guid_buffer_num, guid_buffer_count;
    for (handle_count = 0; handle_count<handles_num && Argc>1; ++handle_count) {
        gBS->ProtocolsPerHandle(controller_handles[handle_count], &guid_buffer, &guid_buffer_num);
        Print(L"Handle[%d]\n", handle_count);
        for (guid_buffer_count = 0; guid_buffer_count < guid_buffer_num; ++guid_buffer_count) {
            Print(L"ProtocolGuid[%d]: %g\n",  guid_buffer_count, guid_buffer[guid_buffer_count]);  // use %g output protocol guid
        }
        Print(L"\n");
    }

    gBS->FreePool(controller_handles);

    return 0;
}