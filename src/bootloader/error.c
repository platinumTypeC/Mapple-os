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

/**
 * @brief Check the status of EFI_STATUS expression given
 * Mostly function calls for the most part
 * 
 * @param input 
 * @param string_to_handle 
 * @return VOID 
 */
EFI_STATUS say_efi_status(
	IN EFI_STATUS input,
	IN CHAR16* string_to_handle
){
	if (EFI_ERROR(input)){
		Print(string_to_handle,
				get_efi_error_message(input));
		
		return input;
	};

	return EFI_SUCCESS;
};
