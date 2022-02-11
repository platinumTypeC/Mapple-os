#pragma once

#include <efi.h>
#include <efilib.h>
#include <efiapi.h>
#include <elf.h>

#include "error.h"
#include "config.h"

EFI_STATUS load_segment(IN EFI_FILE* const kernel_img_file,
	IN EFI_PHYSICAL_ADDRESS const segment_file_offset,
	IN UINTN const segment_file_size,
	IN UINTN const segment_memory_size,
	IN EFI_PHYSICAL_ADDRESS const segment_virtual_address);
EFI_STATUS load_kernel(
	IN EFI_FILE* const root_file_system,
	IN CHAR16* const kernel_image_filename
);