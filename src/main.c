#include "include/efi.h"
#include "include/efilib.h"

EFI_STATUS
EFIAPI
efi_mai(
    EFI_HANDLE ImageHandle,
    EFI_SYSTEM_TABLE* SystemTable
) {
    InitializeLib(ImageHandle, SystemTable);
    Print(L"Hello World! \n");
    return EFI_SUCCESS;
}