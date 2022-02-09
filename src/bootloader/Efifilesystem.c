#include "include/Efifilesystem.h"

EFI_FILE_HANDLE GetVolume(EFI_HANDLE image)
{
	EFI_LOADED_IMAGE *loaded_image = NULL;                  /* image interface */
	EFI_GUID lipGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;      /* image interface GUID */
	
	/* get the loaded image protocol interface for our "image" */
	uefi_call_wrapper(BS->HandleProtocol, 3, image, &lipGuid, (void **) &loaded_image);
	/* get the volume handle */
	return LibOpenRoot(loaded_image->DeviceHandle);
};


EFI_FILE_HANDLE LoadFile(CHAR16* path, EFI_FILE_HANDLE Volume){
	EFI_FILE_HANDLE FileHandle;

	uefi_call_wrapper(Volume->Open, 5, Volume, &FileHandle, path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY | EFI_FILE_HIDDEN | EFI_FILE_SYSTEM);

	return FileHandle;
};