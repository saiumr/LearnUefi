#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseCryptLib.h>

EFI_STATUS
EFIAPI
EncryptTextBySha256 (
  IN   CHAR8  *Text,
  OUT  UINT8  *DigestData,
  IN   UINTN  DigestDataSize
);

VOID
PrintSha256DigestData (
  IN UINT8  *DigestData
);


EFI_STATUS
EFIAPI
UefiMain(
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
)
{
  EFI_STATUS Status;
  UINT8 Digest[64];
  CHAR8 *OriginText = "a1@Byosoft";

  Status = EncryptTextBySha256(OriginText, Digest, 64);  // >32
  if ( EFI_ERROR(Status) ) {
    Print(L"- Failed -\n");
    return Status;
  }

  PrintSha256DigestData(Digest);
  
  Print(L"- Done -\n");
  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
EncryptTextBySha256 (
  IN  CHAR8  *Text,
  OUT UINT8  *DigestData,         
  IN  UINTN  DigestDataSize   // SHA256 has 32 Bytes unsigned int data
)
{
  UINTN TextSize = 0;
  UINTN Sha256TextSize = 0;
  VOID* Sha256Text = NULL;

  TextSize = AsciiStrLen(Text);
  ZeroMem(DigestData, DigestDataSize);
  
  Sha256TextSize = Sha256GetContextSize();
  Sha256Text = AllocatePool(Sha256TextSize);
  
  // Init
  if ( !Sha256Init(Sha256Text) ) {
    Print(L"Error: Sha256Init()\n");
    return EFI_ABORTED;
  }

  // Update
  if ( !Sha256Update(Sha256Text, Text, TextSize) ) {
    Print(L"Error: Sha256Update()\n");
    return EFI_ABORTED;
  }

  // Final
  if ( !Sha256Final(Sha256Text, DigestData) ) {
    Print(L"Error: Sha256Final()\n");
    return EFI_ABORTED;
  }

  FreePool(Sha256Text);

  return EFI_SUCCESS;
}

VOID
PrintSha256DigestData (
  IN UINT8  *DigestData
)
{
  UINTN Index;

  gST->ConOut->SetAttribute(gST->ConOut, EFI_CYAN);
  for (Index = 0; Index < SHA256_DIGEST_SIZE; ++Index) {
    Print(L"%x", DigestData[Index]);
  }
  Print(L"\n");
  gST->ConOut->SetAttribute(gST->ConOut, EFI_LIGHTGRAY);
}

