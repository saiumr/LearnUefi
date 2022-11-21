/** @file
  UP_Audio protocol used by playing, reset, turn volume, it is a device driver
  ! watch out: AC97 has been replaced by HD Audio, here are just some files of framework of implementing device driver.
  Truly example in Book/book/audio
**/


#ifndef __UP_AUDIO_H__
#define __UP_AUDIO_H__

#include <Uefi.h>

#define EFI_AUDIO_PROTOCOL_GUID \
    { 0x542d5351, 0xbc0b, 0x72d2, {0x8e, 0x4f, 0x0, 0xa0, 0xc9, 0x69, 0x72, 0x3b } }
#define AUDIO_PROTOCOL EFI_AUDIO_PROTOCOL_GUID

typedef struct _EFI_AUDIO_PROTOCOL EFI_AUDIO_PROTOCOL;
typedef EFI_AUDIO_PROTOCOL EFI_AUDIO;

typedef
EFI_STATUS
(EFIAPI *EFI_AC97_RESET)(
  IN EFI_AUDIO_PROTOCOL *This
  );

typedef
EFI_STATUS
(EFIAPI *EFI_AC97_PLAY)(
  IN EFI_AUDIO_PROTOCOL *This,
  IN UINT8              *PcmData,
  IN UINT32             Format,      // Audio format
  IN UINTN              Size         // Bytes count of PcmData
  );

typedef
EFI_STATUS
(EFIAPI *EFI_AC97_VOLUME)(
  IN EFI_AUDIO_PROTOCOL *This,
  IN UINT32             Increase,     // +/-
  IN OUT UINT32         *NewVolume    // if *NewVolume >= 0 then Volume = *NewVolume else Volume = Increase + *NewVolume (Maybe..)
  );

#define EFI_AUDIO_PROTOCOL_REVISION 0x00000001
#define EFI_AUDIO_INTERFACE_REVISION EFI_AUDIO_PROTOCOL_REVISION

struct _EFI_AUDIO_PROTOCOL {
  UINT64            Revision;
  EFI_AC97_RESET    Reset;
  EFI_AC97_PLAY     Play;
  EFI_AC97_VOLUME   Volume;
  EFI_EVENT         WaitForEndEvent;
};

typedef EFI_GUID gEfiAudioProtocolGuid;

#endif
