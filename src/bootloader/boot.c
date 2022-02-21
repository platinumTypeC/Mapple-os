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
#include <mapple/config.h>
#include <mapple/types.h>

EFI_HANDLE m_IH;
EFI_SYSTEM_TABLE* m_ST = NULL;
EFI_FILE_PROTOCOL* RTFileSystem = NULL;
EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* SimpleFsProtocol = NULL;
EFI_GRAPHICS_OUTPUT_PROTOCOL* gGOPProtocol = NULL;

BootInfo_t gbootInfo;

#if MAPPLE_DEBUG != 0
	#define DebugPrint(...) Print(L"Debug: "__VA_ARGS__)
#else
	#define DebugPrint(...)
#endif

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
LoadFile(
	CHAR16* FileName,
	EFI_PHYSICAL_ADDRESS *FileAddr,
	UINTN *FilePageSize
){
	DebugPrint(L"Starting to Load File\n");
	EFI_FILE_PROTOCOL *File;
	CHECKER(
		uefi_call_wrapper(RTFileSystem->Open, 5, RTFileSystem, &File, 
			FileName, EFI_FILE_MODE_READ, 0)
		,
		L"Unable to Open File path !, error: %s\n"
	);

  	UINTN FileInfoBufferSize = sizeof(EFI_FILE_INFO) + sizeof(CHAR16) * StrLen(FileName) + 2;
  	UINT8 FileInfoBuffer[FileInfoBufferSize];

  	CHECKER(
		uefi_call_wrapper(File->GetInfo, 4, File, &gEfiFileInfoGuid, 
			&FileInfoBufferSize, FileInfoBuffer)
		,
		L"Unable to get File Info !, error: %s\n"
	);

	DebugPrint(L"Got file Info\n");
  
	EFI_FILE_INFO *FileInfo = (EFI_FILE_INFO*)FileInfoBuffer;
	UINTN FileSize = FileInfo->FileSize;


	DebugPrint(L"FileSize=%llu\n",FileSize);
	*FilePageSize = (FileSize + 4095) / 4096;
	*FileAddr = 0;

	CHECKER(
		uefi_call_wrapper(BS->AllocatePages, 4, AllocateAnyPages, 
			EfiLoaderData, *FilePageSize, FileAddr)
		,
		L"Unable to Allocate Pages for File !, error: %s\n"
	);

	DebugPrint(L"Allocated pages for File\n");

	CHECKER(
		uefi_call_wrapper(File->Read, 3, File, &FileSize,
    		(VOID *)*FileAddr)
		,
		L"Unable to Read File !, error: %s\n"
	);

	DebugPrint(L"Done Reading File\n");

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
        void *buffer = NULL;
        
		CHECKER(
			uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, encompassing_size, &buffer)
			,
			L"Unable to Allocate Pool for memoryMap !, error: %s\n"
		);

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

	// Get Font file and load it into memory
	EFI_PHYSICAL_ADDRESS FontFileMemLocation;
	UINTN FontFilePageBufferSize;
	
	CHECKER(
		LoadFile(FONT_FILE_PATH, &FontFileMemLocation, &FontFilePageBufferSize)
		,
		L"Unable to Load Font File !, error: %s\n"
	);
	
	return EFI_SUCCESS;
};