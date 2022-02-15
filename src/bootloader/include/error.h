#pragma once

#include <efi.h>
#include <efilib.h>
#include <mapple/config.h>

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

#define CHECKER(expression, handleString) { EFI_STATUS result = expression;if (result != EFI_SUCCESS){ Print(handleString, get_efi_error_message(result) );return result; }}