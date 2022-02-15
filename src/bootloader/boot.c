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

typedef struct s_boot_info {
	EFI_MEMORY_DESCRIPTOR* memory_map;
	UINTN memory_map_size;
	UINTN memory_map_descriptor_size;
	UINTN FrameBufferBaseAddress;
	UINTN HorizontalResolution;
	UINTN VerticalResolution;
	UINTN PixelsPerScanLine;;
} Kernel_Boot_Info;

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
	void (*kernel_entry)(Kernel_Boot_Info* boot_info);
	Kernel_Boot_Info boot_info;

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

#if MAPPLE_DEBUG != 0
	Print(L"Exting Booting Service into kernel...\n");
#endif
	
	kernel_entry = (void (*)(Kernel_Boot_Info*))*KernelEnteryPoint;


	boot_info.memory_map = memory_map;
	boot_info.memory_map_size = memory_map_size;
	boot_info.memory_map_descriptor_size = descriptor_size;
	boot_info.HorizontalResolution = get_gop_protocol()->Mode->Info->HorizontalResolution;
	boot_info.VerticalResolution = get_gop_protocol()->Mode->Info->VerticalResolution;
	boot_info.FrameBufferBaseAddress = get_gop_protocol()->Mode->FrameBufferBase;
	boot_info.PixelsPerScanLine = get_gop_protocol()->Mode->Info->PixelsPerScanLine;

	// There is a status error but it works. so I will Probably re write this later
	gBS->ExitBootServices(ImageHandle, memory_map_key);

#if MAPPLE_DEBUG != 0
	Print(L"Debug: Entring kernel\n");
#endif

	kernel_entry(&boot_info);
#if MAPPLE_DEBUG != 0 
	Print(L"Debug: This Here means your kernel is faulty..\n\r");
#endif

    return EFI_SUCCESS;
};