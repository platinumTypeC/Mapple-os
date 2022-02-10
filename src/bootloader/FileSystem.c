#include "include/FileSystem.h"

EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSystem;
EFI_FILE* root_file_system;

EFI_STATUS init_file_system_service(void)
{
	#ifdef MAPPLE_DEBUG
		Print(L"Debug: Initialising File System service\n");
	#endif

	EFI_STATUS status = uefi_call_wrapper(gBS->LocateProtocol, 3,
		&gEfiSimpleFileSystemProtocolGuid, NULL, &FileSystem);
	if(EFI_ERROR(status)) {
		Print(L"Error: Error locating Simple File System Protocol: %s\n",
			get_efi_error_message(status));

		return status;
	}

	#ifdef MAPPLE_DEBUG
		Print(L"Debug: Located Simple File System Protocol\n");
	#endif

	status = uefi_call_wrapper(FileSystem->OpenVolume, 2,
		FileSystem, &root_file_system);
	if(EFI_ERROR(status)) {
		Print(L"Fatal Error: Error opening root volume: %s\n",
			get_efi_error_message(status));

		return status;
	}
	
    #ifdef MAPPLE_DEBUG
		Print(L"Debug: File System Initialized \n");
	#endif

	return status;
}