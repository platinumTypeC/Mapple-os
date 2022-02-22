/**
 * @file boot.c
 * @author Ansh Mathur (worldanvilbild@gmail.com)
 * @brief The main Bootsec of the Mapple-os kernel
 * @version 0.1
 * @date 2022-02-21
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <efi.h>
#include <efilib.h>
#include <efiapi.h>
#include "include/elf.h"
#include "include/error.h"
#include <mapple/types.h>
#include <mapple/config.h>

EFI_HANDLE m_IH;
EFI_SYSTEM_TABLE* m_ST = NULL;
EFI_FILE_PROTOCOL* RTFileSystem = NULL;
EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* SimpleFsProtocol = NULL;
EFI_GRAPHICS_OUTPUT_PROTOCOL* gGOPProtocol = NULL;

BootInfo_t gbootInfo;

EFI_STATUS
EFIAPI
InitFileSystem(){
	CHECKER(
		uefi_call_wrapper(gBS->LocateProtocol, 3, 
			&gEfiSimpleFileSystemProtocolGuid, NULL, &SimpleFsProtocol)
		,
		L"Unable to Locate FileSystem !, error: %s\n"
	);
	DebugPrint(L"Located Filesystem\n");
	CHECKER(
		uefi_call_wrapper(SimpleFsProtocol->OpenVolume, 2, SimpleFsProtocol,
			&RTFileSystem)
		,
		L"Failed to Open System Root !, error: %s\n"
	);
	DebugPrint(L"Opened volume\n");
	return EFI_SUCCESS;
};

EFI_STATUS
EFIAPI
OpenFile(
	IN CHAR16* FileName,
	OUT EFI_FILE** File
){
	CHECKER(
		uefi_call_wrapper(RTFileSystem->Open, 5, RTFileSystem, File, 
			FileName, EFI_FILE_MODE_READ, 0)
		,
		L"Unable to Open File path !, error: %s\n"
	);
	DebugPrint(L"Opened File\n");

	return EFI_SUCCESS;
};

EFI_STATUS
EFIAPI
LoadFont(){
	// Get Font file and load it into memory
	EFI_FILE* FontFile;

	CHECKER(
		OpenFile(FONT_FILE_PATH, &FontFile)
		,
		L"Unable to OpenFile !, error: %s\n"
	);

	PSF1_HEADER_t* fontHeader = (PSF1_HEADER_t*)AllocatePool(sizeof(PSF1_FONT_t));

	UINTN size = sizeof(PSF1_HEADER_t);

	CHECKER(
		uefi_call_wrapper(FontFile->Read, 3, FontFile, 
			&size, fontHeader)
		,
		L"Unable to Read FontFile !, error: %s\n"
	);

	UINTN glyphBufferSize = fontHeader->charsize * 256;
	if (fontHeader->mode == 1) { //512 glyph mode
		glyphBufferSize = fontHeader->charsize * 512;
	}

	void* glyphBuffer;
	{
		CHECKER(
			uefi_call_wrapper(FontFile->SetPosition, 2, 
				FontFile, sizeof(PSF1_HEADER))
			,
			L"Unable to Set Position of Font !, error: %s\n"
		);

		glyphBuffer = AllocatePool(glyphBufferSize);

		CHECKER(
			uefi_call_wrapper(FontFile->Read, 3, FontFile, 
				&glyphBufferSize, glyphBuffer)
			,
			L"Unable to Read FontFile !, error: %s\n"
		);
	}
	gbootInfo.defaultFont.glyphBuffer = glyphBuffer;
	gbootInfo.defaultFont.psf1_Header = fontHeader;
	return EFI_SUCCESS;
};

EFI_STATUS
EFIAPI
InitGOP(){
	EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	CHECKER(
		uefi_call_wrapper(BS->LocateProtocol, 3, 
			&gopGuid, NULL, (void**)&gGOPProtocol)
		,
		L"Error: Unable to locate GOP !, error: %s\n\r"
	);
	gbootInfo.frameBuffer->BaseAddress = (void*)gGOPProtocol->Mode->FrameBufferBase;
	gbootInfo.frameBuffer->BufferSize = gGOPProtocol->Mode->FrameBufferSize;
	gbootInfo.frameBuffer->Width = gGOPProtocol->Mode->Info->HorizontalResolution;
	gbootInfo.frameBuffer->Height = gGOPProtocol->Mode->Info->VerticalResolution;
	gbootInfo.frameBuffer->PixelsPerScanLine = gGOPProtocol->Mode->Info->PixelsPerScanLine;

	return EFI_SUCCESS;
};

EFI_STATUS
EFIAPI
InitAcpiTable(){
	DebugPrint(L"Interating through Tables on device to find Vendor table...\n");
	for (uint64_t Index = 0; Index < gST->NumberOfTableEntries; Index++) {
		// if this is vendor table do this
		if (CompareGuid (&AcpiTableGuid, &(gST->ConfigurationTable[Index].VendorGuid))) {
			gbootInfo.rsdp = gST->ConfigurationTable[Index].VendorTable;
			return EFI_SUCCESS;
		}
	};
	DebugPrint(L"Unable to find it...\n");
	return EFI_NOT_FOUND;
};

EFI_STATUS
EFIAPI
IniGetMemoryDescriptor(){
    EFI_STATUS status = EFI_SUCCESS;

    EFI_MEMORY_DESCRIPTOR* memory_map = NULL;
    UINT32 version = 0;
    UINTN map_key = 0;
    UINTN descriptor_size = 0;
    UINTN memory_map_size = 0;

    status = uefi_call_wrapper(BS->GetMemoryMap, 5, &memory_map_size, memory_map, &map_key, &descriptor_size, &version);
    // descriptor size and version should be correct values.  memory_map_size should be needed size.

    if (status == EFI_BUFFER_TOO_SMALL) {
        UINTN encompassing_size = memory_map_size + (2 * descriptor_size);
        void *buffer = AllocatePool(encompassing_size);
        
		memory_map = (EFI_MEMORY_DESCRIPTOR*) buffer;
		memory_map_size = encompassing_size;

		CHECKER(
			uefi_call_wrapper(BS->GetMemoryMap, 5, &memory_map_size, memory_map, &map_key, &descriptor_size, &version)
			,
			L"Unable to get Memory Map !, error: %s\n"
		);
	} else {
		DebugPrint(L"this should be IMPOSSIBLE\n");
	}

	gbootInfo.memoryMap.mMap = memory_map;
	gbootInfo.memoryMap.mMapSize = memory_map_size;
	gbootInfo.memoryMap.mMapKey = map_key;
	gbootInfo.memoryMap.descVersion = version;
	gbootInfo.memoryMap.descSize = descriptor_size;

	return EFI_SUCCESS;
};

EFI_STATUS
EFIAPI
InitailizeAllProtocols(
	EFI_HANDLE ImageHandle,
	EFI_SYSTEM_TABLE* SystemTable
){
	m_IH = ImageHandle;
	m_ST = SystemTable;

	DebugPrint(L"Initalizing the file protocol\n");
	CHECKER(
		InitFileSystem()
		,
		L"Error: Failed to Initialize FileSystem Protocol !, %s\n"
	);
	DebugPrint(L"Done Initializing file protocol\n");
	
	DebugPrint(L"Starting the Location GOP\n");
	CHECKER(
		InitGOP()
		,
		L"Unable to Locate GOP !, error: %s\n"
	);
	DebugPrint(L"Done Locating GOP\n");

	DebugPrint(L"Starting to get Vendor Config Table\n");
	CHECKER(
		InitAcpiTable()
		,
		L"Unable to Get System Vendor Config Table !, error: %s\n"
	);
	DebugPrint(L"Got Vendor System Config Table\n");

	DebugPrint(L"Geting Memory descriptor\n");
	CHECKER(
		IniGetMemoryDescriptor()
		,
		L"Unable to get Memory Deskriptor !, error: %s"
	);
	DebugPrint(L"Done Geting Memory Descriptor\n");
	DebugPrint(L"Loading Default Font\n");
	CHECKER(
		LoadFont()
		,
		L"Unable to Load Font File !, error: %s\n"
	);
	DebugPrint(L"Done Loading Default Font\n");
	return EFI_SUCCESS;
};

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

	#ifdef DEBUG
		DebugPrint(L"Debug: Setting file pointer to segment "
			"offset '0x%llx'\n", segment_file_offset);
	#endif

	status = uefi_call_wrapper(kernel_img_file->SetPosition, 2,
		kernel_img_file, segment_file_offset);
	if(EFI_ERROR(status)) {
		DebugPrint(L"Error: Error setting file pointer position to "
			L"segment offset: %s\n", get_efi_error_message(status));

		return status;
	}

	#ifdef DEBUG
		DebugPrint(L"Debug: Allocating %lu pages at address '0x%llx'\n",
			segment_page_count, segment_virtual_address);
	#endif

	status = uefi_call_wrapper(gBS->AllocatePages, 4,
		AllocateAnyPages, EfiLoaderData, segment_page_count,
		(EFI_PHYSICAL_ADDRESS*)segment_virtual_address);
	if(EFI_ERROR(status)) {
		DebugPrint(L"Error: Error allocating pages for ELF segment: %s\n",
			get_efi_error_message(status));

		return status;
	}

	if(segment_file_size > 0) {
		buffer_read_size = segment_file_size;

		#ifdef DEBUG
			DebugPrint(L"Debug: Allocating segment buffer with size '0x%llx'\n",
				buffer_read_size);
		#endif

		status = uefi_call_wrapper(gBS->AllocatePool, 3,
			EfiLoaderCode, buffer_read_size, (VOID**)&program_data);
		if(EFI_ERROR(status)) {
			DebugPrint(L"Error: Error allocating kernel segment buffer: %s\n",
				get_efi_error_message(status));

			return status;
		}

		#ifdef DEBUG
			DebugPrint(L"Debug: Reading segment data with file size '0x%llx'\n",
				buffer_read_size);
		#endif

		status = uefi_call_wrapper(kernel_img_file->Read, 3,
			kernel_img_file, &buffer_read_size, (VOID*)program_data);
		if(EFI_ERROR(status)) {
			DebugPrint(L"Error: Error reading segment data: %s\n",
				get_efi_error_message(status));

			return status;
		}

		#ifdef DEBUG
			DebugPrint(L"Debug: Copying segment to memory address '0x%llx'\n",
				segment_virtual_address);
		#endif

		status = uefi_call_wrapper(gBS->CopyMem, 3,
			segment_virtual_address, program_data, segment_file_size);
		if(EFI_ERROR(status)) {
			DebugPrint(L"Error: Error copying program section "
				L"into memory: %s\n", get_efi_error_message(status));

			return status;
		}

		#ifdef DEBUG
			DebugPrint(L"Debug: Freeing program section data buffer\n");
		#endif

		status = uefi_call_wrapper(gBS->FreePool, 1, program_data);
		if(EFI_ERROR(status)) {
			DebugPrint(L"Error: Error freeing program section: %s\n",
				get_efi_error_message(status));

			return status;
		}
	}

	// As per ELF Standard, if the size in memory is larger than the file size
	// the segment is mandated to be zero filled.
	// For more information on Refer to ELF standard page 34.
	zero_fill_start = segment_virtual_address + segment_file_size;
	zero_fill_count = segment_memory_size - segment_file_size;

	if(zero_fill_count > 0) {
		#ifdef DEBUG
			DebugPrint(L"Debug: Zero-filling %llu bytes at address '0x%llx'\n",
				zero_fill_count, zero_fill_start);
		#endif

		status = uefi_call_wrapper(gBS->SetMem, 3,
			zero_fill_start, zero_fill_count, 0);
		if(EFI_ERROR(status)) {
			DebugPrint(L"Error: Error zero filling segment: %s\n",
				get_efi_error_message(status));

			return status;
		}
	}

	return EFI_SUCCESS;
};

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
		DebugPrint(L"Fatal Error: No program segments to load ");
		DebugPrint(L"in Kernel image\n");

		return EFI_INVALID_PARAMETER;
	}

	#ifdef DEBUG
		DebugPrint(L"Debug: Loading %u segments\n", n_program_headers);
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
		DebugPrint(L"Fatal Error: No loadable program segments ");
		DebugPrint(L"found in Kernel image\n");

		return EFI_NOT_FOUND;
	}

	return EFI_SUCCESS;
};

/**
 * @brief The bootloader entry point.
 * most of the code is here.
 * 
 * @param ImageHandle 
 * @param SystemTable 
 * @return EFI_STATUS 
 */
EFI_STATUS
EFIAPI
efi_main(
	EFI_HANDLE ImageHandle,
	EFI_SYSTEM_TABLE* SystemTable
){
	InitializeLib(ImageHandle, SystemTable);
	InitailizeAllProtocols(ImageHandle, SystemTable);

	EFI_FILE* kernelFile;

	CHECKER(
		OpenFile(KERNEL_EXECUTABLE_PATH, &kernelFile)
		,
		L"Unable to load kernel.efi !, error: %s\n"
	);


	UINT8* elf_identity_buffer = NULL;
	CHECKER(
		read_elf_identity(kernelFile, &elf_identity_buffer)
		,
		L"Unable to read elf identity !, error: %s\n"
	);

	Elf_File_Class file_class = elf_identity_buffer[EI_CLASS];
	CHECKER(
		validate_elf_identity(elf_identity_buffer)
		,
		L"ELF identity of kernel.elf is not valid 0x10IMPOSIBLE !, error: %s\n"
	);

	FreePool(elf_identity_buffer);

	VOID* kernel_header = NULL;
	VOID* kernel_program_headers = NULL;
	CHECKER(
		read_elf_file(kernelFile, file_class, &kernel_header, &kernel_program_headers)
		,
		L"Unable to read elf from kernel file\n"
	);

	EFI_PHYSICAL_ADDRESS* kernel_entry_point = NULL;
	if(file_class == ELF_FILE_CLASS_32) {
		*kernel_entry_point = ((Elf32_Ehdr*)kernel_header)->e_entry;
	} else if(file_class == ELF_FILE_CLASS_64) {
		*kernel_entry_point = ((Elf64_Ehdr*)kernel_header)->e_entry;
	}

	CHECKER(
		load_program_segments(kernelFile, file_class,
			kernel_header, kernel_program_headers)
		,
		L"while Loding the kernel segments !, error: %s\n"
	);

	DebugPrint("Entering the kernel\n");

	void (*kernel_entry)(BootInfo*);
	kernel_entry = (void (*)(BootInfo_t*))*kernel_entry_point;

	kernel_entry(&gbootInfo);

	DebugPrint(L"This here should never be Reached\n");
	DebugPrint(L"This also means that the kernel is faulty\n");

	return EFI_SUCCESS;
};