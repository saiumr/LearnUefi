#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

INTN 
ShellAppMain (
  IN UINTN Argc, 
  IN CHAR16 **Argv
  ) 
{
  Print(L"Argc: %d, path: %s\n", Argc, Argv[0]);
  gST->ConOut->OutputString(gST->ConOut, L"Hello New ShellAppMain!\n");
  return 0;
}