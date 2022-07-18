#include <Uefi.h>
#include <Pi/PiDxeCis.h>  // must be in front of prototol header file
#include <PiDxe.h>
#include <Protocol/MpService.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

EFI_STATUS
UefiMain(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_MP_SERVICES_PROTOCOL *mp = NULL;
    EFI_PROCESSOR_INFORMATION mp_info;
    UINTN count;
    UINTN pnum, penum;
    EFI_STATUS Status;

    Status = gBS->LocateProtocol(&gEfiMpServiceProtocolGuid, NULL, (VOID**)&mp);
    if (EFI_ERROR(Status)) {
        Print(L"Cannot locate protocol\n");
        Print(L"- Failed -\n");
        return EFI_ABORTED;
    }

    Status = mp->GetNumberOfProcessors(mp, &pnum, &penum);
    if (EFI_ERROR(Status)) {
        Print(L"Cannot get number of processors.\n");
        Print(L"- Failed -\n");
        return EFI_ABORTED;
    }

    for (count = 0; count < pnum; ++count) {
        mp->GetProcessorInfo(mp, count, &mp_info);
        Print(L"Processor[%d] CPUID: 0x%x\n", count, mp_info.ProcessorId);
    }

    Print(L"- Done -\n");
    return EFI_SUCCESS;
}

