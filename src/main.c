#include "include/efi.h"
#include "include/efilib.h"

EFI_STATUS
EFIAPI
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);

    EFI_STATUS Status;

    EFI_INPUT_KEY key;

    uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, L"Mapple os");
    uefi_call_wrapper(ST->ConIn->Reset, 2, ST->ConIn, FALSE);

    while ((Status = ST->ConIn->ReadKeyStroke(ST->ConIn, &key)) == EFI_NOT_READY);

    return Status;
}