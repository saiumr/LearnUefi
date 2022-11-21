#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/DiskInfo.h>
#include <IndustryStandard/Atapi.h>

EFI_GUID gEfiDiskInfoAhciInterfaceGuid  =  EFI_DISK_INFO_AHCI_INTERFACE_GUID;
EFI_GUID gEfiDiskInfoIdeInterfaceGuid   =  EFI_DISK_INFO_IDE_INTERFACE_GUID;
EFI_GUID gEfiDiskInfoNvmeInterfaceGuid  =  EFI_DISK_INFO_NVME_INTERFACE_GUID;
EFI_GUID gEfiDiskInfoScsiInterfaceGuid  =  EFI_DISK_INFO_SCSI_INTERFACE_GUID;
EFI_GUID gEfiDiskInfoSdMmcInterfaceGuid =  EFI_DISK_INFO_SD_MMC_INTERFACE_GUID;
EFI_GUID gEfiDiskInfoUfsInterfaceGuid   =  EFI_DISK_INFO_UFS_INTERFACE_GUID;
EFI_GUID gEfiDiskInfoUsbInterfaceGuid   =  EFI_DISK_INFO_USB_INTERFACE_GUID;


INTN
ShellAppMain (
  IN UINTN Argc, 
  IN CHAR16** Argv
  )
{
    EFI_HANDLE *disk_handles = NULL;
    UINTN disk_num, disk_count;
    gBS->LocateHandleBuffer(
        ByProtocol, 
        &gEfiDiskInfoProtocolGuid,
        NULL,
        &disk_num,
        &disk_handles
    );

    EFI_DISK_INFO_PROTOCOL *single_disk_protocol = NULL;
    ATA_IDENTIFY_DATA *identify_data = NULL;
    UINT32 identify_buffer_size = sizeof(ATA_IDENTIFY_DATA);
    EFI_STATUS status;
    for (disk_count = 0; disk_count < disk_num; ++disk_count) {
        gBS->OpenProtocol(
            disk_handles[disk_count],
            &gEfiDiskInfoProtocolGuid,
            (VOID**)&single_disk_protocol,
            gImageHandle,
            NULL,
            EFI_OPEN_PROTOCOL_GET_PROTOCOL
        );

        Print(L"Device[%d] GUID: %g", disk_count, single_disk_protocol->Interface);
        if (CompareGuid(&single_disk_protocol->Interface, &gEfiDiskInfoAhciInterfaceGuid)) {
            Print(L" AHCI HDD\n");
            status = single_disk_protocol->Identify(
                                            single_disk_protocol,
                                            identify_data,
                                            &identify_buffer_size
                                            );
            if (EFI_ERROR(status)) {
                Print(L"Something wrong.\n");
            }
            else {
                Print(L"ModelName: %a\n", identify_data->ModelName);
            }
            continue;
        }
        if (CompareGuid(&single_disk_protocol->Interface, &gEfiDiskInfoIdeInterfaceGuid)) {
            Print(L" IDE HDD\n");
            status = single_disk_protocol->Identify(
                                            single_disk_protocol,
                                            identify_data,
                                            &identify_buffer_size
                                            );
            if (EFI_ERROR(status)) {
                Print(L"Something wrong.\n");
            }
            else {
                Print(L"ModelName: %a\n", identify_data->ModelName);
            }
            continue;
        }
        if (CompareGuid(&single_disk_protocol->Interface, &gEfiDiskInfoNvmeInterfaceGuid)) {
            Print(L" NVME HDD\n");
            status = single_disk_protocol->Identify(
                                            single_disk_protocol,
                                            identify_data,
                                            &identify_buffer_size
                                            );
            if (EFI_ERROR(status)) {
                Print(L"Something wrong.\n");
            }
            else {
                Print(L"ModelName: %a\n", identify_data->ModelName);
            }
            continue;
        }
        if (CompareGuid(&single_disk_protocol->Interface, &gEfiDiskInfoScsiInterfaceGuid)) {
            Print(L" SCSI HDD\n");
            status = single_disk_protocol->Identify(
                                            single_disk_protocol,
                                            identify_data,
                                            &identify_buffer_size
                                            );
            if (EFI_ERROR(status)) {
                Print(L"Something wrong.\n");
            }
            else {
                Print(L"ModelName: %a\n", identify_data->ModelName);
            }
            continue;
        }
        if (CompareGuid(&single_disk_protocol->Interface, &gEfiDiskInfoUsbInterfaceGuid)) {
            Print(L" USB HDD\n");
            status = single_disk_protocol->Identify(
                                            single_disk_protocol,
                                            identify_data,
                                            &identify_buffer_size
                                            );
            if (EFI_ERROR(status)) {
                Print(L"Something wrong.\n");
            }
            else {
                Print(L"ModelName: %a\n", identify_data->ModelName);
            }
            continue;
        }
        if (CompareGuid(&single_disk_protocol->Interface, &gEfiDiskInfoSdMmcInterfaceGuid)) {
            Print(L" SDMMC HDD\n");
            status = single_disk_protocol->Identify(
                                            single_disk_protocol,
                                            identify_data,
                                            &identify_buffer_size
                                            );
            if (EFI_ERROR(status)) {
                Print(L"Something wrong.\n");
            }
            else {
                Print(L"ModelName: %a\n", identify_data->ModelName);
            }
            continue;
        }
        if (CompareGuid(&single_disk_protocol->Interface, &gEfiDiskInfoUfsInterfaceGuid)) {
            Print(L" UFS HDD\n");
            status = single_disk_protocol->Identify(
                                            single_disk_protocol,
                                            identify_data,
                                            &identify_buffer_size
                                            );
            if (EFI_ERROR(status)) {
                Print(L"Something wrong.\n");
            }
            else {
                Print(L"ModelName: %a\n", identify_data->ModelName);
            }
            continue;
        }

    }

    return 0;
} 
