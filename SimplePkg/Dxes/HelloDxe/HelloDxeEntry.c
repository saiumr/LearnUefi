/** @file
  Implementation file for HelloDxe protocol.
**/

#include "HelloDxe.h"

// EFI_HELLO_DXE_PROTOCOL gHelloProtocol = {
// 	1,
// 	HelloDxeSayHello
// };

EFI_STATUS
EFIAPI
PrivateDataInitialize()
{
	HELLO_DXE_PRIVATE_DATA* private = &gHelloDxePrivate;

	// gHelloDxePrivate is a private data object that stored in memory
	// private = AllocateZeroPool(sizeof(HELLO_DXE_PRIVATE_DATA));
	// if (private == NULL) return EFI_OUT_OF_RESOURCES;

	private->Signature = HELLO_DXE_PRIVATE_DATA_SIGNATURE;
	private->Word      = L"OMG! MY HELLO PROTOCOL!";
	private->HelloDxe.Revision = 1;
	private->HelloDxe.SayHello = HelloDxeSayHello;

	return EFI_SUCCESS;
}

VOID
HelloDxeSayHello (
  IN EFI_HELLO_DXE_PROTOCOL* This
  )
{
	HELLO_DXE_PRIVATE_DATA* private = HELLO_DXE_PRIVATE_DATA_FROM_THIS(This);

	DEBUG((EFI_D_ERROR, "Hello!!!\n"));   // No output
	DEBUG((EFI_D_ERROR, "Hello!!!\n"));
	DEBUG((EFI_D_ERROR, "Hello!!!\n"));
	DEBUG((EFI_D_INFO, "Hello!!!\n"));
	DEBUG((EFI_D_INFO, "Hello!!!\n"));
	gST->ConOut->OutputString(gST->ConOut, private->Word);
}

EFI_STATUS
EFIAPI
HelloDxeInitialize(
	IN EFI_HANDLE           ImageHandle,
	IN EFI_SYSTEM_TABLE* 		SystemTable
)
{
	EFI_STATUS Status;
	HELLO_DXE_PRIVATE_DATA* private = &gHelloDxePrivate;

	Status = PrivateDataInitialize();
	Status = gBS->InstallProtocolInterface(
						&ImageHandle,
						&gEfiHelloDxeProtocolGuid,
						EFI_NATIVE_INTERFACE,
						&private->HelloDxe
						);

	return Status;
}
