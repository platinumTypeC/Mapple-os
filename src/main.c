#include "include/efi.h"
#include "include/efilib.h"

#include "kernel.h"

EFI_STATUS
EFIAPI
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);
    EFI_STATUS status;
    // EFI_INPUT_KEY Key;

    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    
    status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL, (void**)&gop);
    if(EFI_ERROR(status))
        Print(L"Unable to locate GOP\n");
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
    UINTN SizeOfInfo, numModes, nativeMode;
    
    status = uefi_call_wrapper(gop->QueryMode, 4, gop, gop->Mode==NULL?0:gop->Mode->Mode, &SizeOfInfo, &info);
    // this is needed to get the current video mode
    if (status == EFI_NOT_STARTED)
        status = uefi_call_wrapper(gop->SetMode, 2, gop, 0);
    if(EFI_ERROR(status)) {
        Print(L"Unable to get native mode\n");
    } else {
        nativeMode = gop->Mode->Mode;
        numModes = gop->Mode->MaxMode;
    }

    for (int i = 0; i < numModes; i++) {
        status = uefi_call_wrapper(gop->QueryMode, 4, gop, i, &SizeOfInfo, &info);
        Print(L"mode %03d width %d height %d format %x%s\n",
            i,
            info->HorizontalResolution,
            info->VerticalResolution,
            info->PixelFormat,
            i == nativeMode ? "(current)" : ""
        );
    }
    return status;
}