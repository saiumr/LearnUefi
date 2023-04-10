#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/ReportStatusCodeLib.h>

EFI_STATUS
EFIAPI
HelloDxeEntry(
	IN EFI_HANDLE           ImageHandle,
	IN EFI_SYSTEM_TABLE* SystemTable
)
{
	DEBUG((EFI_D_ERROR, "Hello!!!\n"));
	DEBUG((EFI_D_ERROR, "Hello!!!\n"));
	DEBUG((EFI_D_ERROR, "Hello!!!\n"));
	DEBUG((EFI_D_ERROR, "Hello!!!\n"));
	DEBUG((EFI_D_ERROR, "Hello!!!\n"));
	DEBUG((EFI_D_ERROR, "Hello!!!\n"));
	DEBUG((EFI_D_ERROR, "Hello!!!\n"));
	DEBUG((EFI_D_ERROR, "Hello!!!\n"));
	DEBUG((EFI_D_ERROR, "Hello!!!\n"));
	return EFI_SUCCESS;
}
