#pragma once

#include <efi.h>
#include <efilib.h>
#include "config.h"

/**
 * @brief Gets a formatted string explaining an EFI error.
 * Returns a C string representing an EFI error encountered in a human
 * interpretable format.
 * @param[in] status    The EFI_STATUS encountered.
 * @return              A const string explaining the error message.
 */
const CHAR16* get_efi_error_message(IN EFI_STATUS const status);
EFI_STATUS say_efi_status(
	IN EFI_STATUS input,
	IN CHAR16* string_to_handle
);

#define CHECKER(...) { if (say_efi_status(__VA_ARGS__) != EFI_SUCCESS){ return EFI_UNSUPPORTED; }}