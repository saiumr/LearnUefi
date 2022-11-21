#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/IoLib.h> 
// #include <Protocol/PciIo.h>
// #include <Protocol/PciRootBridgeIo.h>

#define PCI_IO_ADDR  0xCF8
#define PCI_IO_DATA  0xCFC
#define BUS_MAX      0xFF
#define DEV_MAX      0x1F
#define FUNC_MAX     0x07
#define MAKE_CONFIG_ADDR(bus, dev, func, offset)\
(UINT32)(0x80000000 | (bus)<<16 | (dev)<<11 | (func)<<8 | (offset)&0xFC)

// IO specified 32bit
EFI_STATUS PciIOWrite32(IN UINT32 io_addr, IN UINT32 data);
UINT32 PciIORead32(IN UINT32 io_data);
EFI_STATUS PciEnumeration();

EFI_STATUS
UefiMain (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
    )
{
    EFI_STATUS Status;
    Status = PciEnumeration();

    if (!EFI_ERROR(Status)) {
        Print(L"- Done -\n");
    }
    else {
        Print(L"- Failed -\n");
    }

    return Status;
}


EFI_STATUS PciEnumeration() {
    EFI_STATUS Status;
    UINT32 bus_id;
    UINT32 dev_id;
    UINT32 func_id;
    UINT32 data;

    gST->ConOut->SetAttribute(gST->ConOut, EFI_RED);
    Print(L"BUS      DEV      FUNC     Vendor   Device   Class   \n");
    Print(L"---      ---      ----     ------   ------   -----   \n");
    gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW);

    for (bus_id = 0; bus_id <= BUS_MAX; ++bus_id) {
        for (dev_id = 0; dev_id <= DEV_MAX; ++dev_id) {
            for (func_id = 0; func_id <= FUNC_MAX; ++func_id) {
                
                // Vendor&Device ID
                Status = PciIOWrite32(PCI_IO_ADDR, MAKE_CONFIG_ADDR(bus_id, dev_id, func_id, 0x0));
                if (!EFI_ERROR(Status)) {
                    data = PciIORead32(PCI_IO_DATA);
                    if ((data&0xffff) != 0xffff) {
                        Print(L"%02d       %02d       %02d       0x%04x   0x%04x   ",\
                              bus_id, dev_id, func_id, (data&0xffff), ((data>>16)&0xffff));
                    }
                    else {
                        continue;
                    }
                }
                else {
                    Print(L"Get Vendor&Device ID Error.\n");
                    return EFI_ABORTED;
                }

                // Class Code
                Status = PciIOWrite32(PCI_IO_ADDR, MAKE_CONFIG_ADDR(bus_id, dev_id, func_id, 0x8));
                if (!EFI_ERROR(Status)) {
                    data = PciIORead32(PCI_IO_DATA);
                    Print(L"0x%06x\n", ((data>>8)&0xffffff));
                }
                else {
                    Print(L"Get Class Code Error.\n");
                    return EFI_ABORTED;
                }

            }
        }
    }

    gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE);
    return EFI_SUCCESS;
}

UINT32 PciIORead32(IN UINT32 io_data) {
    return IoRead32(io_data);
}

EFI_STATUS PciIOWrite32(IN UINT32 io_addr, IN UINT32 data) {
    EFI_STATUS Status;
    UINT32 ret_data;
    ret_data = IoWrite32(io_addr, data);

    if (ret_data != data) {
        Status = EFI_ABORTED;
        Print(L"PciWrite32 Function Error.\n");
    }
    else {
        Status = EFI_SUCCESS;
    }

    return Status;
}
