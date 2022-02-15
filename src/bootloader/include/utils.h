#pragma once

#include <efi.h>
#include <efilib.h>
#include <mapple/config.h>

EFI_STATUS disableWatchDogTimer(void);
EFI_STATUS ResetConsole(EFI_SYSTEM_TABLE* SystemTable);