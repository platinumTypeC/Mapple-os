#include <efi.h>
#include <efilib.h>
#include <efiapi.h>
#include <elf.h>

#include "include/error.h"
#include "include/serialService.h"
#include "include/utils.h"
#include "include/graphics.h"
#include "include/FileSystem.h"

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
    EFI_HANDLE ImageHandle,
    EFI_SYSTEM_TABLE *SystemTable
){
    InitializeLib(ImageHandle, SystemTable);


	disableWatchDogTimer();
	ResetConsole(SystemTable);
	
	if (init_serial_service() != EFI_SUCCESS){
		Print(L"Error: Unable to init serial service \n\r");
		return EFI_UNSUPPORTED;
	};

	if (init_graphics_output_service(ImageHandle) != EFI_SUCCESS){
		Print(L"Error: Unable to init Graphics output protocol\n\r");
		return EFI_UNSUPPORTED;
	};

	if (init_file_system_service() != EFI_SUCCESS){
		Print(L"Error: Unable to init file system");
		return EFI_UNSUPPORTED;
	}

#ifdef MAPPLE_DEBUG
	else{
		Print(L"Kernel Loaded Successfully \n\r");
	};
#endif
#ifdef MAPPLE_DEBUG
	Print(L"Debug: Done\n");
#endif

    return EFI_SUCCESS;
};