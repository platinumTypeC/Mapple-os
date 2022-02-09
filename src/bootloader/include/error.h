#pragma once

#include <efi.h>
#include <efilib.h>

/**
 * @brief Gets a formatted string explaining an EFI error.
 * Returns a C string representing an EFI error encountered in a human
 * interpretable format.
 * @param[in] status    The EFI_STATUS encountered.
 * @return              A const string explaining the error message.
 */
const CHAR16* get_efi_error_message(IN EFI_STATUS const status);