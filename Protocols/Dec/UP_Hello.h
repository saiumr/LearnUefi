/** @file
  UP_Hello protocol print string on screen
**/

#ifndef __HELLO_PROTOCOL_H__
#define __HELLO_PROTOCOL_H__

//#include <Uefi.h>  // Delete eventually

// 1
#define EFI_HELLO_PROTOCOL_GUID \
  { 0x672d5171, 0xab0b, 0x72d2, {0x8e, 0x4f, 0x0, 0xa0, 0xc9, 0x69, 0x72, 0x3b } }
// Protocol GUID name defined in EFI1.1
#define HELLO_PROTOCOL EFI_HELLO_PROTOCOL_GUID

// 2
typedef struct _EFI_HELLO_PROTOCOL EFI_HELLO_PROTOCOL;
// Protocol defined in EFI1.1
typedef EFI_HELLO_PROTOCOL EFI_HELLO;

// 3
typedef
EFI_STATUS
(EFIAPI *EFI_GREETING)(
  IN EFI_HELLO_PROTOCOL         *This,
  IN CHAR16                     *String
  );


#define EFI_HELLO_PROTOCOL_REVISION  0x00000001
// Revision defined in EFI1.1
#define EFI_HELLO_INTERFACE_REVISION  EFI_HELLO_PROTOCOL_REVISION

// 4
struct _EFI_HELLO_PROTOCOL {
  UINT32 Revision;
  EFI_GREETING    Hello;
};

// 5
extern EFI_GUID gEfiHelloProtocolGuid;

#endif  // __HELLO_PROTOCOL_H__