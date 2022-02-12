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
#include "include/config.h"

typedef struct s_boot_info {
	EFI_MEMORY_DESCRIPTOR* memory_map;
	UINTN memory_map_size;
	UINTN memory_map_descriptor_size;
} Kernel_Boot_Info;

EFI_STATUS get_memory_map(
	OUT VOID** memory_map,
	OUT UINTN* memory_map_size,
	OUT UINTN* memory_map_key,
	OUT UINTN* descriptor_size,
	OUT UINT32* descriptor_version)
{
	/** Program status. */
	EFI_STATUS status;

	status = uefi_call_wrapper(gBS->GetMemoryMap, 5,
		memory_map_size, *memory_map, memory_map_key,
		descriptor_size, descriptor_version);
	if(EFI_ERROR(status)) {
		// This will always fail on the first attempt, this call will return the
		// required buffer size.
		if(status != EFI_BUFFER_TOO_SMALL) {
			Print(L"Fatal Error: Error getting memory map size: %s\n",
				get_efi_error_message(status));
		}
	}

	#ifdef DEBUG
		Print(L"Debug: Allocating memory map\n");
	#endif

	status = uefi_call_wrapper(gBS->AllocatePool, 3,
		EfiLoaderData, *memory_map_size, (VOID**)memory_map);
	if(EFI_ERROR(status)) {
		Print(L"Fatal Error: Error allocating memory map buffer: %s\n",
			get_efi_error_message(status));

		return status;
	}

	status = uefi_call_wrapper(gBS->GetMemoryMap, 5,
		memory_map_size, *memory_map, memory_map_key,
		descriptor_size, descriptor_version);
	if(EFI_ERROR(status)) {
		Print(L"Fatal Error: Error getting memory map: %s\n",
			get_efi_error_message(status));

		return status;
	}
#if MAPPLE_DEBUG != 0
	Print(L"Debug: Allocated memory map buffer at: 0x%llx "
		L"with size: %llu\n", *memory_map, *memory_map_size);
#endif

	return EFI_SUCCESS;
}
/**
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
	Print(L"Debug: Set Kernel Entry Point to: '0x%llx'\n", *KernelEnteryPoint);
	Print(L"Debug: Getting memory map and exiting boot services\n");
#endif

	CHECKER(
		get_memory_map((VOID**)memory_map, &memory_map_size,
			&memory_map_key, &descriptor_size, &descriptor_version),
		
		L"Unable to get memory Map, error: %s\n\r"
	);

#if MAPPLE_DEBUG != 0 
	Print(L"Debug: This should never be reached\n\r");
#endif

    return EFI_SUCCESS;
};