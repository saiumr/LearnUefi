#include "UP_Audio.h"

//
// EFI Component Name Protocol
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME_PROTOCOL  gAudioComponentName = {
  AC97ComponentNameGetDriverName,
  AC97ComponentNameGetControllerName,
  "eng"
};

//
// EFI Component Name 2 Protocol
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME2_PROTOCOL  gAudioComponentName2 = {
  (EFI_COMPONENT_NAME2_GET_DRIVER_NAME)AC97ComponentNameGetDriverName,
  (EFI_COMPONENT_NAME2_GET_CONTROLLER_NAME)AC97ComponentNameGetControllerName,
  "en"
};

EFI_STATUS
EFIAPI
AC97ComponentNameGetDriverName (
  IN  EFI_COMPONENT_NAME_PROTOCOL  *This,
  IN  CHAR8                        *Language,
  OUT CHAR16                       **DriverName
  )
{

}

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
