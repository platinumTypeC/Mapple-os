#pragma once

#include <stddef.h>
#include <stdint.h>
#include <efi.h>
#include <efilib.h>
#include <efiapi.h>
#include <mapple/config.h>

#include "error.h"
#include "FileSystem.h"
#include "elf.h"

EFI_STATUS load_kernel(
    OUT EFI_PHYSICAL_ADDRESS* KernelEntryPoint
);