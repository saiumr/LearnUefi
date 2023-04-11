/** @file
  Head file for HelloDxe protocol implement.
**/

#ifndef _HELLO_DXE_MODULE_H_
#define _HELLO_DXE_MODULE_H_

#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/DebugLib.h>

#include <Protocol/HelloDxe.h>

#define HELLO_DXE_PRIVATE_DATA_SIGNATURE  SIGNATURE_32('H', 'E', 'L', 'O')

typedef struct
{
  UINTN                     Signature;
  EFI_HELLO_DXE_PROTOCOL    HelloDxe;
  CHAR16                    *Word;
} HELLO_DXE_PRIVATE_DATA;
STATIC HELLO_DXE_PRIVATE_DATA gHelloDxePrivate;

// Transformation from protocol [This] pointer to private data field
// So...according to the same protocol [this] pointer, private data from the same region can always be obtained
// return private_data struct head address according to HelloDxe, a is protocol [This] pointer
// the signature used to ensure here is right protocol (maybe there are one or more same name file)
#define HELLO_DXE_PRIVATE_DATA_FROM_THIS(a) CR(a, HELLO_DXE_PRIVATE_DATA, HelloDxe, HELLO_DXE_PRIVATE_DATA_SIGNATURE)

VOID
HelloDxeSayHello (
  IN EFI_HELLO_DXE_PROTOCOL* This
  );

#endif