#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
//#include <Protocol/AcpiTable.h>
#include <Guid/Acpi.h>
#include <IndustryStandard/Acpi.h>

EFI_STATUS
EFIAPI
EnumerateConfigurationTable();

VOID
EFIAPI
SetTextColor(
  IN INT32 Color
);

VOID
EFIAPI
PrintTreeLine(
  IN CHAR8 *TitleString,
  IN UINTN   LayerCount,
  IN BOOLEAN Start
);

VOID
EFIAPI
PrintACPITable(
  IN EFI_ACPI_6_4_ROOT_SYSTEM_DESCRIPTION_POINTER *Root
);

EFI_STATUS
EFIAPI
DisplayACPITableInfo();


EFI_STATUS
UefiMain(
  IN EFI_HANDLE    ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystableTable
)
{
  EFI_STATUS Status = 0;
  //Status = EnumerateConfigurationTable();
  Status = DisplayACPITableInfo();

  if (!EFI_ERROR(Status)) {
    Print(L"\n- Done -\n");
  }
  else {
    Print(L"\n- Failed -\n");
  }
  return Status;
}


EFI_STATUS
EFIAPI
DisplayACPITableInfo()
{
  EFI_GUID                                        AcpiTableGuid = ACPI_TABLE_GUID;
  EFI_GUID                                        EfiAcpiTableGuid = EFI_ACPI_TABLE_GUID;
  EFI_CONFIGURATION_TABLE                         *ConfigurationTable = NULL;
  EFI_ACPI_6_4_ROOT_SYSTEM_DESCRIPTION_POINTER    *Root = NULL;
  UINTN                                           Count;

  ConfigurationTable = gST->ConfigurationTable;
  for (Count = 0; Count < gST->NumberOfTableEntries; ++Count, ++ConfigurationTable) {
    if (&ConfigurationTable->VendorGuid == NULL) {
      Print(L"Configuration Table Guid Error.\n");
      DEBUG((EFI_D_ERROR, "Configuration Table Guid Error.\n"));
      return EFI_ABORTED;
    }
    if (CompareGuid(&ConfigurationTable->VendorGuid, &AcpiTableGuid) || \
      CompareGuid(&ConfigurationTable->VendorGuid, &EfiAcpiTableGuid) ) {
      Root = ConfigurationTable->VendorTable;

      // XSDT defined after revision 2.0
      if (Root->Revision >= EFI_ACPI_6_4_ROOT_SYSTEM_DESCRIPTION_POINTER_REVISION) {
        PrintACPITable(Root);
      }
    }
  }

  return EFI_SUCCESS;
}

VOID
EFIAPI
PrintACPITable(
  IN EFI_ACPI_6_4_ROOT_SYSTEM_DESCRIPTION_POINTER *Root
)
{
  EFI_ACPI_DESCRIPTION_HEADER                     *RSDT = NULL, *XSDT = NULL;
  EFI_ACPI_6_4_FIXED_ACPI_DESCRIPTION_TABLE       *FADT = NULL;  // Get DSDT and FACS(Firmware ACPI Control Structure)
  EFI_ACPI_DESCRIPTION_HEADER                     *DSDT = NULL, *FACS = NULL, *Entry = NULL; 
  UINTN                                           LayerCount = 0;
  CHAR8                                           TableName[20];  // 8 halt
  UINTN                                           EntryListLength;
  UINTN                                           EntryListCount;
  UINTN                                           *EntryListPtr = NULL;

  SetTextColor(EFI_RED);
  Print(L"ACPI table list based on RSD_PTR Revision>=2 (as defined in ACPI 6.4 spec)\n");  // XSDT begin with revision 2.0

  PrintTreeLine("Root", LayerCount, TRUE);
  Print(L"\n");

  // RSDT XSDT
  RSDT = (EFI_ACPI_DESCRIPTION_HEADER*)(UINTN)Root->RsdtAddress;
  XSDT = (EFI_ACPI_DESCRIPTION_HEADER*)Root->XsdtAddress;  // Revision 1.0 have not cast.

  ZeroMem(TableName, sizeof(TableName));
  CopyMem(TableName, &RSDT->Signature, sizeof(UINT32));
  PrintTreeLine(TableName, LayerCount, FALSE);
  Print(L"-->Addr: 0x%08x  Size: %-8d\n", RSDT, RSDT->Length);

  ZeroMem(TableName, sizeof(TableName));
  CopyMem(TableName, (&XSDT->Signature), sizeof(UINT32));
  PrintTreeLine(TableName, LayerCount, FALSE);
  Print(L"-->Addr: 0x%08x  Size: %-8d\n", XSDT, XSDT->Length);

  ++LayerCount;

  // Now Entry List Tables
  // The first entry is always FADT and it is also the header of the list(Entry[0])
  EntryListLength = (XSDT->Length - sizeof(EFI_ACPI_DESCRIPTION_HEADER) ) / sizeof(UINT64);
  EntryListCount  = 0;
  EntryListPtr = (UINTN*)(XSDT+1);
  Entry = (EFI_ACPI_DESCRIPTION_HEADER*)(UINTN)(*EntryListPtr);
  FADT = (EFI_ACPI_6_4_FIXED_ACPI_DESCRIPTION_TABLE*)Entry;
  DSDT = (EFI_ACPI_DESCRIPTION_HEADER*)(UINTN)(FADT->Dsdt);
  FACS = (EFI_ACPI_DESCRIPTION_HEADER*)(UINTN)(FADT->FirmwareCtrl);
  
  ZeroMem(TableName, sizeof(TableName));
  CopyMem(TableName, &Entry->Signature, sizeof(UINT32));
  PrintTreeLine(TableName, LayerCount, FALSE);
  Print(L"-->Addr: 0x%08x  Size: %-8d     ", Entry, Entry->Length);
  SetTextColor(EFI_YELLOW);
  Print(L"Entry: %d\n", EntryListCount);
  SetTextColor(EFI_LIGHTGRAY);

  ++LayerCount;

  ZeroMem(TableName, sizeof(TableName));
  CopyMem(TableName, &DSDT->Signature, sizeof(UINT32));
  PrintTreeLine(TableName, LayerCount, FALSE);
  Print(L"-->Addr: 0x%08x  Size: %-8d\n", DSDT, DSDT->Length);

  ZeroMem(TableName, sizeof(TableName));
  CopyMem(TableName, &FACS->Signature, sizeof(UINT32));
  PrintTreeLine(TableName, LayerCount, FALSE);
  Print(L"-->Addr: 0x%08x  Size: %-8d\n", FACS, FACS->Length);

  --LayerCount;

  // rest entry
  for (EntryListCount = 1; EntryListCount < EntryListLength; ++EntryListCount) {
    Entry = (EFI_ACPI_DESCRIPTION_HEADER*)(UINTN)EntryListPtr[EntryListCount];
    if ( !(&Entry->Signature) ) {
      Print(L"Error: Entry signature NULL.\n");
      DEBUG((EFI_D_ERROR, "[%s][%d]Entry signature NULL, may be use a invalid address\n", __FUNCTION__, __LINE__) );
      return ;
    }

    ZeroMem(TableName, sizeof(TableName));
    CopyMem(TableName, &Entry->Signature, sizeof(UINT32));
    PrintTreeLine(TableName, LayerCount, FALSE);
    Print(L"-->Addr: 0x%08x  Size: %-8d     ", Entry, Entry->Length);
    SetTextColor(EFI_YELLOW);
    Print(L"Entry: %d\n", EntryListCount);
    SetTextColor(EFI_LIGHTGRAY);
  }

  SetTextColor(EFI_LIGHTGRAY);
}

VOID
EFIAPI
PrintTreeLine(
  IN CHAR8 *TitleString,
  IN UINTN   LayerCount,
  IN BOOLEAN Start
)
{
  UINTN Count;

  for (Count = 0; Count < LayerCount; ++Count) {
    Print(L"   ");
  }

  SetTextColor(EFI_WHITE);
  if (!Start) {
    Print(L"|--");
  }

  SetTextColor(EFI_LIGHTCYAN);
  if (TitleString) {
    Print(L"%a  ", TitleString);
  }
  SetTextColor(EFI_LIGHTGREEN);
}

VOID
EFIAPI
SetTextColor(
  IN INT32 Color
)
{
  gST->ConOut->SetAttribute(gST->ConOut, Color);
}

EFI_STATUS
EFIAPI
EnumerateConfigurationTable()
{
  UINTN Count;
  EFI_CONFIGURATION_TABLE *ConfigurationTable = NULL;
  ConfigurationTable = gST->ConfigurationTable;

  Print(L"There are [%d] tables.\n", gST->NumberOfTableEntries);
  for (Count = 0; Count < gST->NumberOfTableEntries && (&ConfigurationTable->VendorGuid); ++Count) {
    Print(L"%g\n", &ConfigurationTable->VendorGuid);
    ++ConfigurationTable;
  }

  return EFI_SUCCESS;
}