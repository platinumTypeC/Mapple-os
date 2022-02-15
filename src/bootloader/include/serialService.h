#pragma once

#include <efi.h>
#include <efilib.h>
#include "error.h"
#include <mapple/config.h>

EFI_STATUS configure_serial_protocol(IN EFI_SERIAL_IO_PROTOCOL* const protocol);
EFI_STATUS init_serial_service(void);