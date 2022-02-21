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

#include <include/elf.h>
#include <include/error.h>
#include <mapple/config.h>


/**
 * print_elf_file_info
 */
VOID print_elf_file_info(IN VOID* const header_ptr,
	IN VOID* const program_headers_ptr)
{
	/**
	 * The header pointer cast to a 32bit ELF Header so that we can read the
	 * header and determine the file type. Then cast to a 64bit header for
	 * specific fields if necessary.
	 */
	Elf32_Ehdr* header = (Elf32_Ehdr*)header_ptr;
	Elf64_Ehdr* header64 = (Elf64_Ehdr*)header_ptr;

	Print(L"Debug: ELF Header Info:\n");

	Print(L"  Magic:                    ");
	UINTN i = 0;
	for(i = 0; i < 4; i++) {
		Print(L"0x%x ", header->e_ident[i]);
	}
	Print(L"\n");

	Print(L"  Class:                    ");
	if(header->e_ident[EI_CLASS] == ELF_FILE_CLASS_32) {
		Print(L"32bit");
	} else if(header->e_ident[EI_CLASS] == ELF_FILE_CLASS_64) {
		Print(L"64bit");
	}
	Print(L"\n");

	Print(L"  Endianness:               ");
	if(header->e_ident[EI_DATA] == 1) {
		Print(L"Little-Endian");
	} else if(header->e_ident[EI_DATA] == 2) {
		Print(L"Big-Endian");
	}
	Print(L"\n");

	Print(L"  Version:                  0x%x\n",
		header->e_ident[EI_VERSION]);

	Print(L"  OS ABI:                   ");
	if(header->e_ident[EI_OSABI] == 0x00) {
		Print(L"System V");
	} else if(header->e_ident[EI_OSABI] == 0x01) {
		Print(L"HP-UX");
	} else if(header->e_ident[EI_OSABI] == 0x02) {
		Print(L"NetBSD");
	} else if(header->e_ident[EI_OSABI] == 0x03) {
		Print(L"Linux");
	} else if(header->e_ident[EI_OSABI] == 0x04) {
		Print(L"GNU Hurd");
	} else if(header->e_ident[EI_OSABI] == 0x06) {
		Print(L"Solaris");
	} else if(header->e_ident[EI_OSABI] == 0x07) {
		Print(L"AIX");
	} else if(header->e_ident[EI_OSABI] == 0x08) {
		Print(L"IRIX");
	} else if(header->e_ident[EI_OSABI] == 0x09) {
		Print(L"FreeBSD");
	} else if(header->e_ident[EI_OSABI] == 0x0A) {
		Print(L"Tru64");
	} else if(header->e_ident[EI_OSABI] == 0x0B) {
		Print(L"Novell Modesto");
	} else if(header->e_ident[EI_OSABI] == 0x0C) {
		Print(L"OpenBSD");
	} else if(header->e_ident[EI_OSABI] == 0x0D) {
		Print(L"OpenVMS");
	} else if(header->e_ident[EI_OSABI] == 0x0E) {
		Print(L"NonStop Kernel");
	} else if(header->e_ident[EI_OSABI] == 0x0F) {
		Print(L"AROS");
	} else if(header->e_ident[EI_OSABI] == 0x10) {
		Print(L"Fenix OS");
	} else if(header->e_ident[EI_OSABI] == 0x11) {
		Print(L"CloudABI");
	}
	Print(L"\n");

	Print(L"  File Type:                ");
	if(header->e_type == 0x00) {
		Print(L"None");
	} else if(header->e_type == 0x01) {
		Print(L"Relocatable");
	} else if(header->e_type == 0x02) {
		Print(L"Executable");
	} else if(header->e_type == 0x03) {
		Print(L"Dynamic");
	} else {
		Print(L"Other");
	}
	Print(L"\n");

	Print(L"  Machine Type:             ");
	if(header->e_machine == 0x00) {
		Print(L"No specific instruction set");
	} else if(header->e_machine == 0x02) {
		Print(L"SPARC");
	} else if(header->e_machine == 0x03) {
		Print(L"x86");
	} else if(header->e_machine == 0x08) {
		Print(L"MIPS");
	} else if(header->e_machine == 0x14) {
		Print(L"PowerPC");
	} else if(header->e_machine == 0x16) {
		Print(L"S390");
	} else if(header->e_machine == 0x28) {
		Print(L"ARM");
	} else if(header->e_machine == 0x2A) {
		Print(L"SuperH");
	} else if(header->e_machine == 0x32) {
		Print(L"IA-64");
	} else if(header->e_machine == 0x3E) {
		Print(L"x86-64");
	} else if(header->e_machine == 0xB7) {
		Print(L"AArch64");
	} else if(header->e_machine == 0xF3) {
		Print(L"RISC-V");
	}
	Print(L"\n");

	if(header->e_ident[EI_CLASS] == ELF_FILE_CLASS_32) {
		Print(L"  Entry point:              0x%lx\n", header->e_entry);
		Print(L"  Program header offset:    0x%lx\n", header->e_phoff);
		Print(L"  Section header offset:    0x%lx\n", header->e_shoff);
		Print(L"  Program header count:     %u\n", header->e_phnum);
		Print(L"  Section header count:     %u\n", header->e_shnum);

		Elf32_Phdr* program_headers = program_headers_ptr;

		Print(L"\nProgram Headers:\n");
		UINTN p = 0;
		for(p = 0; p < header->e_phnum; p++) {
			Print(L"[%u]:\n", p);
			Print(L"  p_type:      0x%lx\n", program_headers[p].p_type);
			Print(L"  p_offset:    0x%lx\n", program_headers[p].p_offset);
			Print(L"  p_vaddr:     0x%lx\n", program_headers[p].p_vaddr);
			Print(L"  p_paddr:     0x%lx\n", program_headers[p].p_paddr);
			Print(L"  p_filesz:    0x%lx\n", program_headers[p].p_filesz);
			Print(L"  p_memsz:     0x%lx\n", program_headers[p].p_memsz);
			Print(L"  p_flags:     0x%lx\n", program_headers[p].p_flags);
			Print(L"  p_align:     0x%lx\n", program_headers[p].p_align);
			Print(L"\n");
		}
	} else if(header->e_ident[EI_CLASS] == ELF_FILE_CLASS_64) {
		Print(L"  Entry point:              0x%llx\n", header64->e_entry);
		Print(L"  Program header offset:    0x%llx\n", header64->e_phoff);
		Print(L"  Section header offset:    0x%llx\n", header64->e_shoff);
		Print(L"  Program header count:     %u\n", header64->e_phnum);
		Print(L"  Section header count:     %u\n", header64->e_shnum);

		Elf64_Phdr* program_headers = program_headers_ptr;

		Print(L"\nDebug: Program Headers:\n");
		UINTN p = 0;
		for(p = 0; p < header64->e_phnum; p++) {
			Print(L"[%u]:\n", p);
			Print(L"  p_type:      0x%lx\n",  program_headers[p].p_type);
			Print(L"  p_flags:     0x%lx\n",  program_headers[p].p_flags);
			Print(L"  p_offset:    0x%llx\n", program_headers[p].p_offset);
			Print(L"  p_vaddr:     0x%llx\n", program_headers[p].p_vaddr);
			Print(L"  p_paddr:     0x%llx\n", program_headers[p].p_paddr);
			Print(L"  p_filesz:    0x%llx\n", program_headers[p].p_filesz);
			Print(L"  p_memsz:     0x%llx\n", program_headers[p].p_memsz);
			Print(L"  p_align:     0x%llx\n", program_headers[p].p_align);
			Print(L"\n");
		}
	}
}


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
	#if MAPPLE_DEBUG != 0
		Print(L"Debug: Setting file pointer to "
			"read executable header\n");
	#endif

	EFI_STATUS status = uefi_call_wrapper(kernel_img_file->SetPosition, 2,
		kernel_img_file, 0);
	if(EFI_ERROR(status)) {
		Print(L"Error: Error setting file pointer position: %s\n",
			get_efi_error_message(status));

		return status;
	}

	if(file_class == ELF_FILE_CLASS_32) {
		buffer_read_size = sizeof(Elf32_Ehdr);
	} else if(file_class == ELF_FILE_CLASS_64) {
		buffer_read_size = sizeof(Elf64_Ehdr);
	} else {
		Print(L"Error: Invalid file class\n", status);
		return EFI_INVALID_PARAMETER;
	}

	#if MAPPLE_DEBUG != 0
		Print(L"Debug: Allocating '0x%lx' for ", buffer_read_size);
		Print(L"kernel executable header buffer\n");
	#endif

	status = uefi_call_wrapper(gBS->AllocatePool, 3,
		EfiLoaderData, buffer_read_size, kernel_header_buffer);
	if(EFI_ERROR(status)) {
		Print(L"Error: Error allocating kernel header buffer: %s\n",
			get_efi_error_message(status));

		return status;
	}

	#if MAPPLE_DEBUG != 0
		Print(L"Debug: Reading kernel executable header\n");
	#endif

	status = uefi_call_wrapper(kernel_img_file->Read, 3,
		kernel_img_file, &buffer_read_size, *kernel_header_buffer);
	if(EFI_ERROR(status)) {
		Print(L"Error: Error reading kernel header: %s\n",
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
	#if MAPPLE_DEBUG != 0
		Print(L"Debug: Setting file offset to '0x%lx' "
			"to read program headers\n", program_headers_offset);
	#endif

	status = uefi_call_wrapper(kernel_img_file->SetPosition, 2,
		kernel_img_file, program_headers_offset);
	if(EFI_ERROR(status)) {
		Print(L"Error: Error setting file pointer position: %s\n",
			get_efi_error_message(status));

		return status;
	}

	// Allocate memory for program headers.
	#if MAPPLE_DEBUG != 0
		Print(L"Debug: Allocating '0x%lx' for program headers buffer\n",
			buffer_read_size);
	#endif

	status = uefi_call_wrapper(gBS->AllocatePool, 3,
		EfiLoaderData, buffer_read_size, kernel_program_headers_buffer);
	if(EFI_ERROR(status)) {
		Print(L"Error: Error allocating kernel "
			"program header buffer: %s\n", get_efi_error_message(status));

		return status;
	}

	#if MAPPLE_DEBUG != 0
		Print(L"Debug: Reading program headers\n");
	#endif

	status = uefi_call_wrapper(kernel_img_file->Read, 3,
		kernel_img_file, &buffer_read_size, *kernel_program_headers_buffer);
	if(EFI_ERROR(status)) {
		Print(L"Error: Error reading kernel program headers: %s\n",
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

	#if MAPPLE_DEBUG != 0
		Print(L"Debug: Setting file pointer position "
			"to read ELF identity\n");
	#endif

	// Reset to the start of the file.
	EFI_STATUS status = uefi_call_wrapper(kernel_img_file->SetPosition, 2,
		kernel_img_file, 0);
	if(EFI_ERROR(status)) {
		Print(L"Error: Error resetting file pointer position: %s\n",
			get_efi_error_message(status));

		return status;
	}

	#if MAPPLE_DEBUG != 0
		Print(L"Debug: Allocating buffer for ELF identity\n");
	#endif

	status = uefi_call_wrapper(gBS->AllocatePool, 3,
		EfiLoaderData, EI_NIDENT, (VOID**)elf_identity_buffer);
	if(EFI_ERROR(status)) {
		Print(L"Error: Error allocating kernel identity buffer: %s\n",
			get_efi_error_message(status));

		return status;
	}

	#if MAPPLE_DEBUG != 0
		Print(L"Debug: Reading ELF identity\n");
	#endif

	status = uefi_call_wrapper(kernel_img_file->Read, 3,
		kernel_img_file, &buffer_read_size, (VOID*)*elf_identity_buffer);
	if(EFI_ERROR(status)) {
		Print(L"Error: Error reading kernel identity: %s\n",
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
		Print(L"Fatal Error: Invalid ELF header\n");
		return EFI_INVALID_PARAMETER;
	}

	if(elf_identity_buffer[EI_CLASS] == ELF_FILE_CLASS_32) {
		#if MAPPLE_DEBUG != 0
			Print(L"Debug: Found 32bit executable\n");
		#endif
	} else if(elf_identity_buffer[EI_CLASS] == ELF_FILE_CLASS_64) {
		#if MAPPLE_DEBUG != 0
			Print(L"Debug: Found 64bit executable\n");
		#endif
	} else {
		Print(L"Fatal Error: Invalid executable\n");

		return EFI_UNSUPPORTED;
	}

	if(elf_identity_buffer[EI_DATA] != 1) {
		Print(L"Fatal Error: Only LSB ELF executables "
			"current supported\n");

		return EFI_INCOMPATIBLE_VERSION;
	}

	return EFI_SUCCESS;
}