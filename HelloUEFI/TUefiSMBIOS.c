#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/Smbios.h>

// Update SMBIOS Type1
EFI_STATUS UpdateSmbiosType1(IN EFI_SMBIOS_PROTOCOL *Smbios) {
    EFI_STATUS                  Status = 0;
    EFI_SMBIOS_HANDLE           SmbiosHandle;
    EFI_SMBIOS_TYPE             SmbiosType;
    EFI_SMBIOS_TABLE_HEADER     *SmbiosHdr;
    SMBIOS_STRUCTURE_POINTER    p;                  // union structure
    UINTN                       StringNum;

    SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
    SmbiosType   = SMBIOS_TYPE_SYSTEM_INFORMATION;  // Type 1
    Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);

    if (!EFI_ERROR (Status)){
        // update board version
        p.Hdr = SmbiosHdr;
        StringNum = p.Type1->ProductName;
        Status = Smbios->UpdateString(Smbios, &SmbiosHandle, &StringNum, "TUefiSMBIOS-PC");
    }
    else {
        return Status;
    }

    return Status;
}

EFI_STATUS UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable) {
    EFI_SMBIOS_PROTOCOL *Smbios;
    EFI_STATUS Status;
    Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, &Smbios);

    if (EFI_ERROR(Status)) {
        return Status;
    }

    UpdateSmbiosType1(Smbios);

    return EFI_SUCCESS;
}