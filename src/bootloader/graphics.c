#include "include/graphics.h"

EFI_GRAPHICS_OUTPUT_PROTOCOL* graphics_output_protocol = NULL;
EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
UINTN SizeOfInfo, numModes, nativeMode;

/**
 * @brief init the graphics output service so that we can finally show something more meaning full 
 * on the screen along with Resizing it
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS init_graphics_output_service(EFI_HANDLE ImageHandle)
{
    EFI_STATUS status;
    status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL, (void**)&graphics_output_protocol);
    if(EFI_ERROR(status)){
        Print(L"Unable to locate GOP\n\r");
        return status;
    }

    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
    UINTN SizeOfInfo, numModes, nativeMode;
    
    status = uefi_call_wrapper(graphics_output_protocol->QueryMode, 4, graphics_output_protocol, graphics_output_protocol->Mode==NULL?0:graphics_output_protocol->Mode->Mode, &SizeOfInfo, &info);
    // this is needed to get the current video mode
    if (status == EFI_NOT_STARTED)
        status = uefi_call_wrapper(graphics_output_protocol->SetMode, 2, graphics_output_protocol, 0);
    if(EFI_ERROR(status)) {
        Print(L"Unable to get native mode\n\r");
    } else {
        nativeMode = graphics_output_protocol->Mode->Mode;
        numModes = graphics_output_protocol->Mode->MaxMode;
    }

    UINT8 mode;
    for (int i = 0; i < numModes; i++) {
        status = uefi_call_wrapper(graphics_output_protocol->QueryMode, 4, graphics_output_protocol, i, &SizeOfInfo, &info);
        Print(L"mode %03d width %d height %d format %x%s\n\r",
            i,
            info->HorizontalResolution,
            info->VerticalResolution,
            info->PixelFormat,
            i == nativeMode ? "(current)" : ""
        );

        if (i == 20){
            mode = i;
        }
    }

    status = uefi_call_wrapper(graphics_output_protocol->SetMode, 2, graphics_output_protocol, mode);
    if(EFI_ERROR(status)) {
        Print(L"Unable to set mode %03d\n\r", mode);
    } else {
        // get framebuffer
        Print(L"Framebuffer address %x size %d, width %d height %d pixelsperline %d\n\r",
        graphics_output_protocol->Mode->FrameBufferBase,
        graphics_output_protocol->Mode->FrameBufferSize,
        graphics_output_protocol->Mode->Info->HorizontalResolution,
        graphics_output_protocol->Mode->Info->VerticalResolution,
        graphics_output_protocol->Mode->Info->PixelsPerScanLine
        );
    }

	return EFI_SUCCESS;
}