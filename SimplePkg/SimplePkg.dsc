[Defines]
  DSC_SPECIFICATION = 0x00010005
  PLATFORM_NAME = Simple
  PLATFORM_GUID = 9fe1d005-460d-46e5-a4e8-55c960885e31
  PLATFORM_VERSION = 1.01
  SUPPORTED_ARCHITECTURES = IA32|X64|EBC|ARM|AARCH64|RISCV64
  BUILD_TARGETS = DEBUG|RELEASE|NOOPT
  SKUID_IDENTIFIER = DEFAULT
  OUTPUT_DIRECTORY = Build/Simple

  DEFINE DEBUG_ENABLE_OUTPUT = TRUE     # Enable Debug
  DEFINE DEBUG_PROPERTY_MASK = 2        # DEBUG PRINT


[LibraryClasses]
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  DebugLib|MdePkg/Library/UefiDebugLibStdErr/UefiDebugLibStdErr.inf
  DebugLib|MdePkg/Library/UefiDebugLibConOut/UefiDebugLibConOut.inf
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  PciLib|MdePkg/Library/BasePciLibCf8/BasePciLibCf8.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf
  FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
  SortLib|MdeModulePkg/Library/UefiSortLib/UefiSortLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  BmpSupportLib|MdeModulePkg/Library/BaseBmpSupportLib/BaseBmpSupportLib.inf
  SafeIntLib|MdePkg/Library/BaseSafeIntLib/BaseSafeIntLib.inf
  ShellCEntryLib|ShellPkg/Library/UefiShellCEntryLib/UefiShellCEntryLib.inf
  LibC|StdLib/LibC/LibC.inf
  LibCType|StdLib/LibC/Ctype/Ctype.inf
  LibLocale|StdLib/LibC/Locale/Locale.inf
  LibMath|StdLib/LibC/Math/Math.inf
  LibSignal|StdLib/LibC/Signal/Signal.inf
  LibStdio|StdLib/LibC/Stdio/Stdio.inf
  LibStdLib|StdLib/LibC/StdLib/StdLib.inf
  LibString|StdLib/LibC/String/String.inf
  LibTime|StdLib/LibC/Time/Time.inf
  LibUefi|StdLib/LibC/Uefi/Uefi.inf
  LibWchar|StdLib/LibC/Wchar/Wchar.inf
  LibGdtoa|StdLib/LibC/gdtoa/gdtoa.inf
  LibGen|StdLib/PosixLib/Gen/LibGen.inf
  DevUtility|StdLib/LibC/Uefi/Devices/daUtility.inf
  DevConsole|StdLib/LibC/Uefi/Devices/daConsole.inf
  LibIIO|StdLib/LibC/Uefi/InteractiveIO/IIO.inf
  LibContainer|StdLib/LibC/Containers/ContainerLib.inf
  UefiRuntimeLib|MdePkg/Library/UefiRuntimeLib/UefiRuntimeLib.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  DevMedia|StdLib/LibC/Uefi/Devices/daShell.inf
  OrderedCollectionLib|MdePkg/Library/BaseOrderedCollectionRedBlackTreeLib/BaseOrderedCollectionRedBlackTreeLib.inf
  GifParserLib|LearnUefi/SimplePkg/Library/GifParserLib/GifParserLib.inf



[Components]
  LearnUefi/SimplePkg/App/HelloUefiPkgApp/HelloUefiPkg.inf
  LearnUefi/Tasks/UefiUSBDeviceEnumeration.inf
  LearnUefi/HelloUEFI/UefiEventMain.inf
  LearnUefi/SimplePkg/App/HiiTest/HiiTest.inf
  LearnUefi/SimplePkg/App/HiiPic/HiiPic.inf
  LearnUefi/SimplePkg/App/HiiPic2/HiiPic2.inf
#  LearnUefi/SimplePkg/giftest/giftest.inf
  LearnUefi/SimplePkg/App/HelloTestStd/HelloWorld.inf
  LearnUefi/SimplePkg/App/GifParserStd/ParserMain.inf
#!include StdLib/StdLib.inc
# !include AppPkg/Applications/Sockets/Sockets.inc