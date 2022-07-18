#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

EFI_STATUS
UefiMain(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SyatemTable
)
{
    EFI_HANDLE *controller_handles = NULL;
    UINTN handles_num, handle_count;

    // get all of handle in an array
    gBS->LocateHandleBuffer(AllHandles, NULL, NULL, &handles_num, &controller_handles);
    for (handle_count = 0; handle_count < handles_num; ++handle_count) {
        Print(L"Handles[%d]: %p\n", handle_count, controller_handles[handle_count]);
    }

    EFI_GUID **guid_buffer = NULL;
    UINTN guid_buffer_num, guid_buffer_count;
    for (handle_count = 0; handle_count<handles_num; ++handle_count) {
        gBS->ProtocolsPerHandle(controller_handles[handle_count], &guid_buffer, &guid_buffer_num);
        Print(L"Handle[%d]\n", handle_count);
        for (guid_buffer_count = 0; guid_buffer_count < guid_buffer_num; ++guid_buffer_count) {
            Print(L"ProtocolGuid[%d]: { %x, %x, %x, { %x, %x, %x, %x, %x, %x, %x, %x } }\n", \
                guid_buffer_count, \
                guid_buffer[guid_buffer_count]->Data1, \
                guid_buffer[guid_buffer_count]->Data2, \
                guid_buffer[guid_buffer_count]->Data3, \
                guid_buffer[guid_buffer_count]->Data4[0], \
                guid_buffer[guid_buffer_count]->Data4[1], \
                guid_buffer[guid_buffer_count]->Data4[2], \
                guid_buffer[guid_buffer_count]->Data4[3], \
                guid_buffer[guid_buffer_count]->Data4[4], \
                guid_buffer[guid_buffer_count]->Data4[5], \
                guid_buffer[guid_buffer_count]->Data4[6], \
                guid_buffer[guid_buffer_count]->Data4[7]
                );
        }
        Print(L"\n");
    }

    gBS->FreePool(controller_handles);
    
    return EFI_SUCCESS;
}