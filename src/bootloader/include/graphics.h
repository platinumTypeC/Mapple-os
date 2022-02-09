#pragma once

#include <efi.h>
#include <efilib.h>
#include "error.h"

typedef struct s_uefi_graphics_service {
	EFI_HANDLE* handle_buffer;
	UINTN handle_count;
} Uefi_Graphics_Service;

Uefi_Graphics_Service GraphicsService;

EFI_STATUS init_graphics_output_service(EFI_HANDLE ImageHandle);