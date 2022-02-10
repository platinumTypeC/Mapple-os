#pragma once

#include <efi.h>
#include <efilib.h>
#include "error.h"
#include "config.h"

typedef struct s_uefi_graphics_service {
	EFI_HANDLE* handle_buffer;
	UINTN handle_count;
} Uefi_Graphics_Service;

Uefi_Graphics_Service GraphicsService;

EFI_STATUS init_graphics_output_service(EFI_HANDLE ImageHandle);
VOID draw_rect(IN EFI_GRAPHICS_OUTPUT_PROTOCOL* const protocol,
	IN const UINT16 _x,
	IN const UINT16 _y,
	IN const UINT16 width,
	IN const UINT16 height,
	IN const UINT32 color
);
EFI_GRAPHICS_OUTPUT_PROTOCOL* get_gop_protocol();