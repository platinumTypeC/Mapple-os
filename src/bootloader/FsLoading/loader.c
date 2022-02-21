#include <include/loader.h>

/**
 * load_segment
 */
EFI_STATUS load_segment(IN EFI_FILE* const kernel_img_file,
	IN EFI_PHYSICAL_ADDRESS const segment_file_offset,
	IN UINTN const segment_file_size,
	IN UINTN const segment_memory_size,
	IN EFI_PHYSICAL_ADDRESS const segment_virtual_address)
{
	/** Program status. */
	EFI_STATUS status;
	/** Buffer to hold the segment data. */
	VOID* program_data = NULL;
	/** The amount of data to read into the buffer. */
	UINTN buffer_read_size = 0;
	/** The number of pages to allocate. */
	UINTN segment_page_count = EFI_SIZE_TO_PAGES(segment_memory_size);
	/** The memory location to begin zero filling empty segment space. */
	EFI_PHYSICAL_ADDRESS zero_fill_start = 0;
	/** The number of bytes to zero fill. */
	UINTN zero_fill_count = 0;

	#if MAPPLE_DEBUG != 0
		Print(L"Debug: Setting file pointer to segment "
			"offset '0x%llx'\n", segment_file_offset);
	#endif

	CHECKER(
		uefi_call_wrapper(kernel_img_file->SetPosition, 2,
			kernel_img_file, segment_file_offset)
		,
		L"Error: Error setting file pointer position to segment offset: %s\n"
	);

	#if MAPPLE_DEBUG != 0
		Print(L"Debug: Allocating %lu pages at address '0x%llx'\n",
			segment_page_count, segment_virtual_address);
	#endif

	CHECKER(
		uefi_call_wrapper(gBS->AllocatePages, 4,
			AllocateAnyPages, EfiLoaderData, segment_page_count,
			(EFI_PHYSICAL_ADDRESS*)segment_virtual_address)
		,
		L"Error: Error allocating pages for ELF segment: %s\n"	
	);

	if(segment_file_size > 0) {
		buffer_read_size = segment_file_size;

		#if MAPPLE_DEBUG != 0
			Print(L"Debug: Allocating segment buffer with size '0x%llx'\n",
				buffer_read_size);
		#endif

		CHECKER(
			uefi_call_wrapper(gBS->AllocatePool, 3,
				EfiLoaderCode, buffer_read_size, (VOID**)&program_data)
			,
			L"Error: Error allocating kernel segment buffer: %s\n"
		);
		
		#if MAPPLE_DEBUG != 0
			Print(L"Debug: Reading segment data with file size '0x%llx'\n",
				buffer_read_size);
		#endif

		CHECKER(
			uefi_call_wrapper(kernel_img_file->Read, 3,
				kernel_img_file, &buffer_read_size, (VOID*)program_data)
			,
			L"Error: Error reading segment data: %s\n"
		);

		#if MAPPLE_DEBUG != 0
			Print(L"Debug: Copying segment to memory address '0x%llx'\n",
				segment_virtual_address);
		#endif

		CHECKER(
			uefi_call_wrapper(gBS->CopyMem, 3,
				segment_virtual_address, program_data, segment_file_size)
			,
			L"Error: Error copying program section into memory: %s\n"
		);

		#if MAPPLE_DEBUG != 0
			Print(L"Debug: Freeing program section data buffer\n");
		#endif

		CHECKER(
			uefi_call_wrapper(gBS->FreePool, 1, program_data)
			,
			L"Error: Error freeing program section: %s\n"
		);
	}

	// As per ELF Standard, if the size in memory is larger than the file size
	// the segment is mandated to be zero filled.
	// For more information on Refer to ELF standard page 34.
	zero_fill_start = segment_virtual_address + segment_file_size;
	zero_fill_count = segment_memory_size - segment_file_size;

	if(zero_fill_count > 0) {
		#if MAPPLE_DEBUG != 0
			Print(L"Debug: Zero-filling %llu bytes at address '0x%llx'\n",
				zero_fill_count, zero_fill_start);
		#endif

		status = uefi_call_wrapper(gBS->SetMem, 3,
			zero_fill_start, zero_fill_count, 0);
		if(EFI_ERROR(status)) {
			Print(L"Error: Error zero filling segment: %s\n",
				get_efi_error_message(status));

			return status;
		}
	}

	return EFI_SUCCESS;
}

/**
 * load_program_segments
 */
EFI_STATUS load_program_segments(IN EFI_FILE* const kernel_img_file,
	IN Elf_File_Class const file_class,
	IN VOID* const kernel_header_buffer,
	IN VOID* const kernel_program_headers_buffer)
{
	/** Program status. */
	EFI_STATUS status;
	/** The number of program headers. */
	UINT16 n_program_headers = 0;
	/** The number of segments loaded. */
	UINT16 n_segments_loaded = 0;
	/** Program section iterator. */
	UINTN p = 0;

	if(file_class == ELF_FILE_CLASS_32) {
		n_program_headers = ((Elf32_Ehdr*)kernel_header_buffer)->e_phnum;
	} else if(file_class == ELF_FILE_CLASS_64) {
		n_program_headers = ((Elf64_Ehdr*)kernel_header_buffer)->e_phnum;
	}

	// Exit if there are no executable sections in the kernel image.
	if(n_program_headers == 0) {
		Print(L"Fatal Error: No program segments to load ");
		Print(L"in Kernel image\n");

		return EFI_INVALID_PARAMETER;
	}

	#if MAPPLE_DEBUG != 0
		Print(L"Debug: Loading %u segments\n", n_program_headers);
	#endif


	if(file_class == ELF_FILE_CLASS_32) {
		/** Program headers pointer. */
		Elf32_Phdr* program_headers = (Elf32_Phdr*)kernel_program_headers_buffer;

		for(p = 0; p < n_program_headers; p++) {
			if(program_headers[p].p_type == PT_LOAD) {
				status = load_segment(kernel_img_file,
					program_headers[p].p_offset,
					program_headers[p].p_filesz,
					program_headers[p].p_memsz,
					program_headers[p].p_vaddr);
				if(EFI_ERROR(status)) {
					// Error has already been printed in the case that loading an
					// individual segment failed.
					return status;
				}

				// Increment the number of segments lodaed.
				n_segments_loaded++;
			}
		}
	} else if(file_class == ELF_FILE_CLASS_64) {
		/** Program headers pointer. */
		Elf64_Phdr* program_headers = (Elf64_Phdr*)kernel_program_headers_buffer;

		for(p = 0; p < n_program_headers; p++) {
			if(program_headers[p].p_type == PT_LOAD){
				status = load_segment(kernel_img_file,
					program_headers[p].p_offset,
					program_headers[p].p_filesz,
					program_headers[p].p_memsz,
					program_headers[p].p_vaddr);
				if(EFI_ERROR(status)) {
					return status;
				}

				n_segments_loaded++;
			}
		}
	}

	// If we have found no loadable segments, raise an exception.
	if(n_segments_loaded == 0) {
		Print(L"Fatal Error: No loadable program segments ");
		Print(L"found in Kernel image\n");

		return EFI_NOT_FOUND;
	}

	return EFI_SUCCESS;
}


EFI_STATUS load_kernel(
	OUT EFI_PHYSICAL_ADDRESS* KernelEntryPoint
){
#if MAPPLE_DEBUG != 0
	Print(L"Debug: Starting to load kernel");
#endif
	EFI_FILE* kernelImageFile;
	UINT8* KernelELFIdentityBuffer = NULL;
	Elf_File_Class FileClass = ELF_FILE_CLASS_NONE;
	VOID* KernelHeader = NULL;
	VOID* KernelProgramHeaders = NULL;

	CHECKER(
		uefi_call_wrapper(get_system_root()->Open, 5,
			get_system_root(), &kernelImageFile, KERNEL_EXECUTABLE_PATH,
			EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY
		)
		,
		L"Error: Unable to get kernel Image, error: %s\n\r"
	);


	CHECKER(
		read_elf_identity(kernelImageFile, &KernelELFIdentityBuffer),
		L"Error: Unable to read headers of kernel.elf, error: %s\n\r"
	);

	FileClass = KernelELFIdentityBuffer[EI_CLASS];

	CHECKER(
		validate_elf_identity(KernelELFIdentityBuffer),
		L"Error: Unable to validate the kernel, error: %s\n\r"
	);

	/**
	 * Everything after this point is causing some strange behaviour.
	 */
#if MAPPLE_DEBUG != 0
	Print(L"Debug: kernel file is valid");
#endif

	CHECKER(
		uefi_call_wrapper(gBS->FreePool, 1, KernelELFIdentityBuffer),
		L"Error: while Freeing the Identity Buffer, error: %s\n\r"
	);

	CHECKER(
		read_elf_file(kernelImageFile, FileClass, &KernelHeader, &KernelProgramHeaders),
		L"Error: Unable to read elf file, error: %s\n\r"
	);

#if MAPPLE_DEBUG != 0
	print_elf_file_info(KernelHeader, KernelProgramHeaders);
#endif

	if(FileClass == ELF_FILE_CLASS_32) {
		*KernelEntryPoint = ((Elf32_Ehdr*)KernelHeader)->e_entry;
	} else if(FileClass == ELF_FILE_CLASS_64) {
		*KernelEntryPoint = ((Elf64_Ehdr*)KernelHeader)->e_entry;
	}

	CHECKER(
		load_program_segments(kernelImageFile, FileClass,
			KernelHeader, KernelProgramHeaders),
		
		L"Error: Unable to load program sections, error: %s\n\r"
	);
#if MAPPLE_DEBUG != 0
	Print(L"Debug: Closing kernel binary\n");
#endif

	CHECKER(
		uefi_call_wrapper(kernelImageFile->Close, 1, kernelImageFile),

		L"Error: Unable to close kernel img file, error: %s\n\r"
	);

#if MAPPLE_DEBUG != 0
	Print(L"Debug: Freeing kernel header buffer\n");
#endif

	FreePool((VOID*)KernelHeader);

#if MAPPLE_DEBUG != 0
	Print(L"Debug: Freeing kernel program header buffer\n");
#endif

	FreePool((VOID*)KernelProgramHeaders);
	
	return EFI_SUCCESS;
};