#include <efi.h>
#include <efilib.h>
#include <efiapi.h>
#include <elf.h>

typedef unsigned long long size_t;

typedef struct {
	void* BaseAddress;
	size_t BufferSize;
	unsigned int Width;
	unsigned int Height;
	unsigned int PixelsPerScanLine;
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

EFI_FILE_HANDLE GetVolume(EFI_HANDLE image)
{
    EFI_LOADED_IMAGE *loaded_image = NULL;                  /* image interface */
    EFI_GUID lipGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;      /* image interface GUID */
    EFI_FILE_IO_INTERFACE *IOVolume;                        /* file system interface */
    EFI_GUID fsGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID; /* file system interface GUID */
    EFI_FILE_HANDLE Volume;                                 /* the volume's interface */
    
    /* get the loaded image protocol interface for our "image" */
    uefi_call_wrapper(BS->HandleProtocol, 3, image, &lipGuid, (void **) &loaded_image);
    /* get the volume handle */
    uefi_call_wrapper(BS->HandleProtocol, 3, loaded_image->DeviceHandle, &fsGuid, (VOID*)&IOVolume);
    uefi_call_wrapper(IOVolume->OpenVolume, 2, IOVolume, &Volume);
    return Volume;
};

EFI_FILE_INFO *
EFIAPI
FileHandleGetInfo (
    IN EFI_FILE_HANDLE  FileHandle
) {
    EFI_FILE_INFO  *FileInfo;
    UINTN          FileInfoSize;
    EFI_STATUS     Status;

    if (FileHandle == NULL) {
        return (NULL);
    }

    //
    // Get the required size to allocate
    //
    FileInfoSize = 0;
    FileInfo     = NULL;
    Print(L"Unable to get file info\r\n");
    Status       = FileHandle->GetInfo ( FileHandle, &gEfiFileInfoGuid, &FileInfoSize, NULL);
    if (Status == EFI_BUFFER_TOO_SMALL) {
        FileInfo = AllocateZeroPool (FileInfoSize);
        if (FileInfo != NULL) {
        Status = FileHandle->GetInfo (
                                FileHandle,
                                &gEfiFileInfoGuid,
                                &FileInfoSize,
                                FileInfo
                                );
        if (EFI_ERROR (Status)) {
            FreePool (FileInfo);
            FileInfo = NULL;
        }
        }
    }

    return (FileInfo);
}

EFI_FILE* LoadFile(CHAR16* location, EFI_FILE_HANDLE Volume){
    EFI_FILE_HANDLE     FileHandle;

    Print(L"starting wish me luck\r\n");
    
    /* open the file */
    uefi_call_wrapper(Volume->Open, 5, Volume, &FileHandle, location, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY | EFI_FILE_HIDDEN | EFI_FILE_SYSTEM);

    Print(L"Opened File \r\n");

    /* read from the file */
    EFI_FILE_INFO*      FileInfo = FileHandleGetInfo(FileHandle);
    UINT64              ReadSize = FileInfo->FileSize;
    UINT8               *Buffer = AllocatePool(ReadSize);
    
    uefi_call_wrapper(FileHandle->Read, 3, FileHandle, &ReadSize, Buffer);

    Print(L"reading from file\r\n");

    return NULL;
};

EFI_STATUS
EFIAPI
efi_main(
    EFI_HANDLE ImageHandle,
    EFI_SYSTEM_TABLE *SystemTable
){
    InitializeLib(ImageHandle, SystemTable);
  
    EFI_FILE_HANDLE Volume = GetVolume(ImageHandle);

    EFI_FILE* file = LoadFile(L"test.txt", Volume);

    Print(L"DONE\r\n");
 
    return EFI_SUCCESS;
};