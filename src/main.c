#include "include/efi.h"
#include "include/efilib.h"

EFI_STATUS
EFIAPI
efi_main(
    EFI_HANDLE ImageHandle,
    EFI_SYSTEM_TABLE* SystemTable
) {
    EFI_STATUS Status;
    EFI_INPUT_KEY Key;

    ST = SystemTable;
    Print(L"Hello World\n");

    return EFI_SUCCESS;
}