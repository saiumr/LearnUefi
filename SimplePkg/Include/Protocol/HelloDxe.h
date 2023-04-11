/** @file
  This is my protocol -> Service driver, only one function it just like say "Hello".
  It's really simple.
**/

#ifndef __PROTOCOL_HELLO_DXE__
#define __PROTOCOL_HELLO_DXE__

#define EFI_HELLO_DXE_PROTOCOL_GUID \
  { 0x754E21F6, 0x56CC, 0x22d4, {0xAA, 0x38, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D } }

typedef struct _EFI_HELLO_DXE_PROTOCOL EFI_HELLO_DXE_PROTOCOL;


// defined a function pointer -> void(EFI_HELLO_DXE_PROTOCOL* This)
typedef
VOID
(EFIAPI *EFI_SAY_HELLO)(
  IN EFI_HELLO_DXE_PROTOCOL* This 
  );


struct _EFI_HELLO_DXE_PROTOCOL
{
  UINT64            Revision;
  EFI_SAY_HELLO     SayHello;
};

extern EFI_GUID gEfiHelloDxeProtocolGuid;

#endif