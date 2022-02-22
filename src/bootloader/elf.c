/**
 * @file elf.c
 * @author ajxs
 * @date Aug 2019
 * @brief Functionality for working with ELF executable files.
 * Contains functionality to assist in loading and validating ELF executable
 * files. This functionality is essential to the ELF executable loader.
 */

#include <efi.h>
#include <efilib.h>

#include "include/elf.h"
#include "include/error.h"

/**
 * read_elf_file
 */
EFI_STATUS read_elf_file(IN EFI_FILE* const kernel_img_file,
	IN Elf_File_Class const file_class,
	OUT VOID** kernel_header_buffer,
	OUT VOID** kernel_program_headers_buffer)
{
	/** The number of bytes to read into the read buffers. */
	UINTN buffer_read_size = 0;
	/** The offset of the program headers in the executable. */
	UINTN program_headers_offset = 0;

	// Reset to start of file.
	#ifdef DEBUG
		DebugPrint(L"Debug: Setting file pointer to "
			"read executable header\n");
	#endif

	EFI_STATUS status = uefi_call_wrapper(kernel_img_file->SetPosition, 2,
		kernel_img_file, 0);
	if(EFI_ERROR(status)) {
		DebugPrint(L"Error: Error setting file pointer position: %s\n",
			get_efi_error_message(status));

		return status;
	}

	if(file_class == ELF_FILE_CLASS_32) {
		buffer_read_size = sizeof(Elf32_Ehdr);
	} else if(file_class == ELF_FILE_CLASS_64) {
		buffer_read_size = sizeof(Elf64_Ehdr);
	} else {
		DebugPrint(L"Error: Invalid file class\n", status);
		return EFI_INVALID_PARAMETER;
	}

	#ifdef DEBUG
		DebugPrint(L"Debug: Allocating '0x%lx' for ", buffer_read_size);
		DebugPrint(L"kernel executable header buffer\n");
	#endif

	status = uefi_call_wrapper(gBS->AllocatePool, 3,
		EfiLoaderData, buffer_read_size, kernel_header_buffer);
	if(EFI_ERROR(status)) {
		DebugPrint(L"Error: Error allocating kernel header buffer: %s\n",
			get_efi_error_message(status));

		return status;
	}

	#ifdef DEBUG
		DebugPrint(L"Debug: Reading kernel executable header\n");
	#endif

	status = uefi_call_wrapper(kernel_img_file->Read, 3,
		kernel_img_file, &buffer_read_size, *kernel_header_buffer);
	if(EFI_ERROR(status)) {
		DebugPrint(L"Error: Error reading kernel header: %s\n",
			get_efi_error_message(status));

		return status;
	}


	if(file_class == ELF_FILE_CLASS_32) {
		program_headers_offset = ((Elf32_Ehdr*)*kernel_header_buffer)->e_phoff;
		buffer_read_size = sizeof(Elf32_Phdr) *
			((Elf32_Ehdr*)*kernel_header_buffer)->e_phnum;
	} else if(file_class == ELF_FILE_CLASS_64) {
		program_headers_offset = ((Elf64_Ehdr*)*kernel_header_buffer)->e_phoff;
		buffer_read_size = sizeof(Elf64_Phdr) *
			((Elf64_Ehdr*)*kernel_header_buffer)->e_phnum;
	}

	// Read program headers.
	#ifdef DEBUG
		DebugPrint(L"Debug: Setting file offset to '0x%lx' "
			"to read program headers\n", program_headers_offset);
	#endif

	status = uefi_call_wrapper(kernel_img_file->SetPosition, 2,
		kernel_img_file, program_headers_offset);
	if(EFI_ERROR(status)) {
		DebugPrint(L"Error: Error setting file pointer position: %s\n",
			get_efi_error_message(status));

		return status;
	}

	// Allocate memory for program headers.
	#ifdef DEBUG
		DebugPrint(L"Debug: Allocating '0x%lx' for program headers buffer\n",
			buffer_read_size);
	#endif

	status = uefi_call_wrapper(gBS->AllocatePool, 3,
		EfiLoaderData, buffer_read_size, kernel_program_headers_buffer);
	if(EFI_ERROR(status)) {
		DebugPrint(L"Error: Error allocating kernel "
			"program header buffer: %s\n", get_efi_error_message(status));

		return status;
	}

	#ifdef DEBUG
		DebugPrint(L"Debug: Reading program headers\n");
	#endif

	status = uefi_call_wrapper(kernel_img_file->Read, 3,
		kernel_img_file, &buffer_read_size, *kernel_program_headers_buffer);
	if(EFI_ERROR(status)) {
		DebugPrint(L"Error: Error reading kernel program headers: %s\n",
			get_efi_error_message(status));

		return status;
	}

	return EFI_SUCCESS;
}


/**
 * read_elf_identity
 */
EFI_STATUS read_elf_identity(IN EFI_FILE* const kernel_img_file,
	OUT UINT8** elf_identity_buffer)
{
	/** The amount of bytes to read into the buffer. */
	UINTN buffer_read_size = EI_NIDENT;

	#ifdef DEBUG
		DebugPrint(L"Debug: Setting file pointer position "
			"to read ELF identity\n");
	#endif

	// Reset to the start of the file.
	EFI_STATUS status = uefi_call_wrapper(kernel_img_file->SetPosition, 2,
		kernel_img_file, 0);
	if(EFI_ERROR(status)) {
		DebugPrint(L"Error: Error resetting file pointer position: %s\n",
			get_efi_error_message(status));

		return status;
	}

	#ifdef DEBUG
		DebugPrint(L"Debug: Allocating buffer for ELF identity\n");
	#endif

	status = uefi_call_wrapper(gBS->AllocatePool, 3,
		EfiLoaderData, EI_NIDENT, (VOID**)elf_identity_buffer);
	if(EFI_ERROR(status)) {
		DebugPrint(L"Error: Error allocating kernel identity buffer: %s\n",
			get_efi_error_message(status));

		return status;
	}

	#ifdef DEBUG
		DebugPrint(L"Debug: Reading ELF identity\n");
	#endif

	status = uefi_call_wrapper(kernel_img_file->Read, 3,
		kernel_img_file, &buffer_read_size, (VOID*)*elf_identity_buffer);
	if(EFI_ERROR(status)) {
		DebugPrint(L"Error: Error reading kernel identity: %s\n",
			get_efi_error_message(status));

		return status;
	}

	return EFI_SUCCESS;
}


/**
 * validate_elf_identity
 */
EFI_STATUS validate_elf_identity(IN UINT8* const elf_identity_buffer)
{
	if((elf_identity_buffer[EI_MAG0] != 0x7F) ||
		(elf_identity_buffer[EI_MAG1] != 0x45) ||
		(elf_identity_buffer[EI_MAG2] != 0x4C) ||
		(elf_identity_buffer[EI_MAG3] != 0x46)) {
		DebugPrint(L"Fatal Error: Invalid ELF header\n");
		return EFI_INVALID_PARAMETER;
	}

	if(elf_identity_buffer[EI_CLASS] == ELF_FILE_CLASS_32) {
		#ifdef DEBUG
			DebugPrint(L"Debug: Found 32bit executable\n");
		#endif
	} else if(elf_identity_buffer[EI_CLASS] == ELF_FILE_CLASS_64) {
		#ifdef DEBUG
			DebugPrint(L"Debug: Found 64bit executable\n");
		#endif
	} else {
		DebugPrint(L"Fatal Error: Invalid executable\n");

		return EFI_UNSUPPORTED;
	}

	if(elf_identity_buffer[EI_DATA] != 1) {
		DebugPrint(L"Fatal Error: Only LSB ELF executables "
			"current supported\n");

		return EFI_INCOMPATIBLE_VERSION;
	}

	return EFI_SUCCESS;
}