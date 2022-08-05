#ifndef _UP_HELLO_H_
#define _UP_HELLO_H_

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include "../Dec/UP_Hello.h"

#define HELLO_PRIVATE_DATA_SIGNATURE SIGNATURE_32('V', 'O', 'I', 'D')

// Sometimes here is not only one protocol
typedef struct {
  UINTN Signature;  // Specified var name. Start two variables are Signature and PROTOCOL
  EFI_HELLO_PROTOCOL Hello;  // produced protocol
                             // consumed protocol (here no such protocol)
  int TotalCallNum;
} HELLO_PRIVATE_DATA;

#define HELLO_PRIVATE_DATA_FROM_THIS(a) CR(a, HELLO_PRIVATE_DATA, Hello, HELLO_PRIVATE_DATA_SIGNATURE)

// Member function declaration
EFI_STATUS
EFIAPI
Greeting(
  IN EFI_HELLO_PROTOCOL    *This,
  IN CHAR16                *String
  );

#endif
