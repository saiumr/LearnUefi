#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>  // use gST access SystemTable
#include <Library/UefiLib.h>

// page
EFI_STATUS TestMMap() {
    EFI_STATUS Status = 0;
    UINTN MemoryMapSize = 0;
    EFI_MEMORY_DESCRIPTOR *MemoryMap = 0;
    UINTN MapKey = 0;
    UINTN DescriptorSize = 0;
    UINT32 DescriptorVersion = 0;

    EFI_MEMORY_DESCRIPTOR *MMap;
    UINTN i = 0;

    // Get BufferSize
    Status = gBS->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);
    if (Status != EFI_BUFFER_TOO_SMALL) {
        return Status;
    }

    // Allocate Memory
    Status = gBS->AllocatePool(EfiBootServicesData, MemoryMapSize, &MemoryMap);

    // Call GetMemoryMap
    Status = gBS->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);

    for (i = 0; i < MemoryMapSize / DescriptorSize; ++i) {
        MMap = (EFI_MEMORY_DESCRIPTOR*)((CHAR8*)MemoryMap + i*DescriptorSize);
        
        Print(L"MemoryMap %4d %10d :", MMap[0].Type, MMap[0].NumberOfPages);
        Print(L"%101x<->", MMap[0].PhysicalStart);
        Print(L"%101x<->", MMap[0].VirtualStart);
    }

    Status = gBS->FreePool(MemoryMap);
    return Status;
}

EFI_STATUS UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable) {
    EFI_STATUS Status;
    Status = TestMMap();

    return Status;
}

