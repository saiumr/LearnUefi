#ifndef _AC97_AUDIO_COMPONENT_NAME_H_
#define _AC97_AUDIO_COMPONENT_NAME_H_

extern EFI_COMPONENT_NAME_PROTOCOL gAudioComponentName;
extern EFI_COMPONENT_NAME_PROTOCOL gAudioComponentName;

EFI_STATUS
EFIAPI
AC97ComponentNameGetDriverName (
  IN  EFI_COMPONENT_NAME_PROTOCOL  *This,
  IN  CHAR8                        *Language,
  OUT CHAR16                       **DriverName
  );

EFI_STATUS
EFIAPI
AC97ComponentNameGetControllerName (
  IN  EFI_COMPONENT_NAME_PROTOCOL  *This,
  IN  EFI_HANDLE                   ControllerHandle,
  IN  EFI_HANDLE                   ChildHandle        OPTIONAL,
  IN  CHAR8                        *Language,
  OUT CHAR16                       **ControllerName
  );

#endif