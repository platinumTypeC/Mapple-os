#include <efi.h>
#include <efilib.h>
#include <efiapi.h>
#include <elf.h>

#include "include/error.h"
#include "include/serialService.h"
#include "include/utils.h"
#include "include/graphics.h"
#include "include/FileSystem.h"
#include "include/loader.h"
#include "include/config.h"

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
    InitializeLib(ImageHandle, SystemTable);

	disableWatchDogTimer();
	ResetConsole(SystemTable);
	
	CHECKER(init_serial_service(), L"Error: Unable to init serial service, Error: %s\n\r");

	CHECKER(init_graphics_output_service(ImageHandle), L"Error: Unable to init Graphics output protocol\n\r");

	if (init_file_system_service() != EFI_SUCCESS){
		Print(L"Error: Unable to init file system\n\r");
		return EFI_UNSUPPORTED;
	}

	if (load_kernel(get_system_root(), KERNEL_EXECUTABLE_PATH) != EFI_SUCCESS){
		return EFI_UNSUPPORTED;
	};

#if MAPPLE_DEBUG != 0 
	Print(L"Error: This should never be reached\n\r");
#endif

    return EFI_SUCCESS;
};