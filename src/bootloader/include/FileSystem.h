#pragma once

/**
 * @brief Init file service.
 */
#include <efi.h>
#include <efilib.h>
#include "error.h"
#include "config.h"

EFI_STATUS init_file_system_service(void);
EFI_FILE* get_system_root();
EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* get_file_system_protocol();