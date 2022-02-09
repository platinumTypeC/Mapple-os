#include <efi.h>
#include <efilib.h>
#include "include/error.h"

CHAR16 error_message_buffer[256];

const CHAR16* get_efi_error_message(IN EFI_STATUS const status)
{
	// Copy the status into the error message buffer.
	StatusToString(error_message_buffer, status);
	return error_message_buffer;
}