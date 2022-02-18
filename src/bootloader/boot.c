#include <efi.h>
#include <efilib.h>
#include <efiapi.h>
#include "include/elf.h"

#include "include/error.h"
#include "include/serialService.h"
#include "include/utils.h"
#include "include/graphics.h"
#include "include/FileSystem.h"
#include "include/loader.h"
#include <mapple/config.h>

typedef struct {
	void* BaseAddress;
	uint64_t BufferSize;
	uint64_t Width;
	uint64_t Height;
	uint64_t PixelsPerScanLine;
} Framebuffer;

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04

typedef struct {
	unsigned char magic[2];
	unsigned char mode;
	unsigned char charsize;
} PSF1_HEADER;

typedef struct {
	PSF1_HEADER* psf1_Header;
	void* glyphBuffer;
} PSF1_FONT;

typedef struct {
	Framebuffer* framebuffer;
	PSF1_FONT* psf1_Font;
	EFI_MEMORY_DESCRIPTOR* mMap;
	uint64_t mMapSize;
	uint64_t mMapDescSize;
	void* rsdp;
} BootInfo_t;

EFI_STATUS LoadFont(PSF1_FONT* fontPs1, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable){
	EFI_FILE* fontImageFile;
	CHECKER(
		uefi_call_wrapper(get_system_root()->Open, 5,
			get_system_root(), &fontImageFile, FONT_FILE_PATH,
			EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY
		)
	,
		L"Error: unable to get Font File Image, error: %s\n\r"
	);
	

	PSF1_HEADER* fontHeader = NULL;

	// Going to go with the assumtion that this does not fail
	uefi_call_wrapper(
		SystemTable->BootServices->AllocatePool,
		5,
		EfiLoaderData,
		sizeof(PSF1_HEADER),
		(void**)fontHeader
	);

	UINTN size = sizeof(PSF1_FONT);

	CHECKER(uefi_call_wrapper(fontImageFile->Read, 4, fontImageFile, &size, fontHeader), L"Error: Unable to read font file, error: %s \n\r");

	if (fontHeader->magic[0] != PSF1_MAGIC0 || fontHeader->magic[1] != PSF1_MAGIC1){
		Print(L"Error: Error The Font file Magic Numbers do not match\n\r");
		return EFI_UNSUPPORTED;
	};


	UINTN glyphBufferSize = fontHeader->charsize * 256;
	if (fontHeader->mode == 1) { //512 glyph mode
		glyphBufferSize = fontHeader->charsize * 512;
	}
	

	void* glyphBuffer;
	{
		CHECKER(
			uefi_call_wrapper(fontImageFile->SetPosition, 4,fontImageFile, sizeof(PSF1_HEADER))
			,
			L"Error: gting glyph info from psf1 font, error: %s\n\r"
		);
		CHECKER(
			uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 4,
				EfiLoaderData, glyphBufferSize, (void**)&glyphBuffer)
			,
			L"Error: Unable to Allocate Pool for glyphBuffer, error: %s\n\r"
		);
		CHECKER(
			uefi_call_wrapper(fontImageFile->Read, 4,
				fontImageFile, &glyphBufferSize, glyphBuffer)
			,
			L"Error: Unable to read glyph from font file, error: %s\n\r"
		);
	};
	

	SystemTable->BootServices->AllocatePool(
		EfiLoaderData,
		sizeof(PSF1_FONT),
		(void**)&fontPs1
	);

	fontPs1->psf1_Header = fontHeader;
	fontPs1->glyphBuffer = glyphBuffer;

	return EFI_SUCCESS;
};


EFI_STATUS
get_memory_map(
	OUT EFI_MEMORY_DESCRIPTOR** out_memory_map,
	OUT UINT32* out_descripter_version,
	OUT UINTN* out_descriptor_size,
	OUT UINTN* out_memory_map_size,
	OUT UINTN* out_map_key
){
    EFI_STATUS status = EFI_SUCCESS;

    EFI_MEMORY_DESCRIPTOR *memory_map = NULL;
    UINT32 version = 0;
    UINTN map_key = 0;
    UINTN descriptor_size = 0;
    UINTN memory_map_size = 0;

    status = uefi_call_wrapper(BS->GetMemoryMap, 5, &memory_map_size, memory_map, &map_key, &descriptor_size, &version);
    // descriptor size and version should be correct values.  memory_map_size should be needed size.

    if (status == EFI_BUFFER_TOO_SMALL) {
        UINTN encompassing_size = memory_map_size + (2 * descriptor_size);
        void *buffer = NULL;
        status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, encompassing_size, &buffer);
        if (status == EFI_SUCCESS) {
            memory_map = (EFI_MEMORY_DESCRIPTOR*) buffer;
            memory_map_size = encompassing_size;

            status = uefi_call_wrapper(BS->GetMemoryMap, 5, &memory_map_size, memory_map, &map_key, &descriptor_size, &version);
            if (status != EFI_SUCCESS) {
                Print(L"Second call to GetMemoryMap failed for some reason.");
			}
		}
	}
	
	*out_memory_map = memory_map;
	*out_descripter_version = version;
	*out_descriptor_size = descriptor_size;
	*out_memory_map_size = memory_map_size;
	*out_map_key = map_key;

	return EFI_SUCCESS;
}

UINTN mystrcmp(CHAR8* a, CHAR8* b, UINTN length){
	for (UINTN i = 0; i < length; i++){
		if (*a != *b) return 0;
	}
	return 1;
}

/**
 * 
 * @brief entry point of the entire os.
 * 
 * @param ImageHandle 
 * @param SystemTable 
 * @return EFI_STATUS 
 */
EFI_STATUS
EFIAPI
efi_main(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
){
	EFI_MEMORY_DESCRIPTOR* memory_map = NULL;
	UINTN memory_map_key = 0;
	UINTN memory_map_size = 0;
	UINTN descriptor_size;
	UINT32 descriptor_version;
	void (*kernel_entry)(BootInfo_t* boot_info);

    InitializeLib(ImageHandle, SystemTable);

	disableWatchDogTimer();
	ResetConsole(SystemTable);
	
	CHECKER(init_serial_service(), L"Error: Unable to init serial service, Error: %s\n\r");

	CHECKER(init_graphics_output_service(ImageHandle), L"Error: Unable to init Graphics output protocol\n\r");

	CHECKER(init_file_system_service(), L"Error: Unable to init file System, error: %s\n\r");

	EFI_PHYSICAL_ADDRESS* KernelEnteryPoint = NULL;
	CHECKER(load_kernel(KernelEnteryPoint), L"Unable to load kernel, error: %s\n\r");

#if MAPPLE_DEBUG != 0
	Print(L"Debug: Set Kernel Entry Point to: '0x%llu'\n", *KernelEnteryPoint);
	Print(L"Debug: Geting Memory Map and Info\n");
#endif

	get_memory_map(&memory_map, &descriptor_version, &descriptor_size, &memory_map_size, &memory_map_key);

	
	kernel_entry = (void (*)(BootInfo_t*))*KernelEnteryPoint;

	BootInfo_t boot_info;

	EFI_CONFIGURATION_TABLE* configTable = SystemTable->ConfigurationTable;
	void* rsdp = NULL; 
	EFI_GUID Acpi2TableGuid = ACPI_20_TABLE_GUID;

	for (UINTN index = 0; index < SystemTable->NumberOfTableEntries; index++){
		if (CompareGuid(&configTable[index].VendorGuid, &Acpi2TableGuid)){
			if (mystrcmp((CHAR8*)"RSD PTR ", (CHAR8*)configTable->VendorTable, 8)){
				rsdp = (void*)configTable->VendorTable;
				//break;
			}
		}
		configTable++;
	}

	boot_info.rsdp = rsdp;

	Framebuffer* framebuffer = NULL;

	framebuffer->BaseAddress = (void*)get_gop_protocol()->Mode->FrameBufferBase;
	framebuffer->BufferSize  = get_gop_protocol()->Mode->FrameBufferSize;
	framebuffer->Width = get_gop_protocol()->Mode->Info->HorizontalResolution;
	framebuffer->Height = get_gop_protocol()->Mode->Info->VerticalResolution;
	framebuffer->PixelsPerScanLine = get_gop_protocol()->Mode->Info->PixelsPerScanLine;

	boot_info.framebuffer = framebuffer;
	
	PSF1_FONT defaultFont;

	CHECKER(LoadFont(&defaultFont, ImageHandle, SystemTable), L"Error: Unable to Load Font FIle Image, error: %s\n\r");

	boot_info.psf1_Font = &defaultFont;

#if MAPPLE_DEBUG != 0
	Print(L"Exting Booting Service into kernel...\n");
#endif

	// There is a status error but it works. so I will Probably re write this later
	gBS->ExitBootServices(ImageHandle, memory_map_key);

#if MAPPLE_DEBUG != 0
	Print(L"Debug: Entring kernel\n");
#endif

	kernel_entry(&boot_info);
#if MAPPLE_DEBUG != 0 
	Print(L"Error: This Here means your kernel is faulty..\n\r");
#endif

    return EFI_SUCCESS;
};