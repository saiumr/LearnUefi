#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>


EFI_STATUS
UefiMain (
  IN EFI_HANDLE          ImageHandle,
  IN EFI_SYSTEM_TABLE    *SystemTable
)
{
  CHAR16 *helloworld = L"HelloWorld";
  Print(L"%a\n", helloworld+3);  
  Print(L"%a\n", helloworld);  
  Print(L"%s\n", helloworld+3);  

  return EFI_SUCCESS;
}