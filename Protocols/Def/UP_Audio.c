#include "UP_Audio.h"   // Why not implement? See UP_Audio.h comment.

// driver private data template
AUDIO_PRIVATE_DATA gAudioPrivateDataTemplate = {
  AUDIO_PRIVATE_DATA_SIGNATURE,
  {
    EFI_AUDIO_PROTOCOL_REVISION,
    AC97Reset,
    AC97Play,
    AC97Volume,
    0
  },
  NULL,
  {0}
};

//
// Driver binding protocol implementation for Audio driver.
//
EFI_DRIVER_BINDING_PROTOCOL  gAudioDriverBinding = {
  AC97DriverBindingSupported,
  AC97DriverBindingStart,
  AC97DriverBindingStop,
  0xa,
  NULL,
  NULL
};

// EFI_DRIVER_BINDING_PROTOCOL
// Supported, Start, Stop
EFI_STATUS
EFIAPI
AC97DriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath OPTIONAL
  )
{

}

EFI_STATUS
EFIAPI
AC97DriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath OPTIONAL
  )
{

}

EFI_STATUS
EFIAPI
AC97DriverBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   ControllerHandle,
  IN  UINTN                        NumberOfChildren,
  IN  EFI_HANDLE                   *ChildHandleBuffer
  )
{

}


// EFI_COMPONENT_NAME_PROTOCOL
EFI_STATUS
EFIAPI
AC97ComponentNameGetDriverName (
  IN  EFI_COMPONENT_NAME_PROTOCOL  *This,
  IN  CHAR8                        *Language,
  OUT CHAR16                       **DriverName
  )
{

}

// EFI_COMPONENT_NAME2_PROTOCOL
EFI_STATUS
EFIAPI
AC97ComponentNameGetControllerName (
  IN  EFI_COMPONENT_NAME_PROTOCOL  *This,
  IN  EFI_HANDLE                   ControllerHandle,
  IN  EFI_HANDLE                   ChildHandle        OPTIONAL,
  IN  CHAR8                        *Language,
  OUT CHAR16                       **ControllerName
  )
{

}


// EFI_AUDIO_PROTOCOL Interface
EFI_STATUS
EFIAPI
AC97Reset(
  IN EFI_AUDIO_PROTOCOL *This
  )
{
  
}

EFI_STATUS
EFIAPI
AC97Play(
  IN EFI_AUDIO_PROTOCOL *This,
  IN UINT8              *PcmData,
  IN UINT32             Format,      // Audio format
  IN UINTN              Size         // Bytes count of PcmData
  )
{

}

EFI_STATUS
EFIAPI
AC97Volume(
  IN EFI_AUDIO_PROTOCOL *This,
  IN UINT32             Increase,     // +/-
  IN OUT UINT32         *NewVolume    // if *NewVolume >= 0 then Volume = *NewVolume else Volume = Increase + *NewVolume (Maybe..)
  )
{

}


// Entry point
EFI_STATUS
EFIAPI
InitializeDiskIo (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  //
  // Install driver model protocol(s).
  //
  Status = EfiLibInstallDriverBindingComponentName2 (
             ImageHandle,
             SystemTable,
             &gAudioDriverBinding,
             ImageHandle,
             &gAudioComponentName,
             &gAudioComponentName2
             );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
