#pragma once

#include <efi.h>
#include <efilib.h>

EFI_STATUS disableWatchDogTimer(void);
EFI_STATUS ResetConsole(EFI_SYSTEM_TABLE* SystemTable);