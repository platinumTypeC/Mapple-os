#pragma once
#include <efi.h>
#include <efilib.h>

EFI_FILE_HANDLE GetVolume(EFI_HANDLE image);
EFI_FILE_HANDLE LoadFile(CHAR16* path, EFI_FILE_HANDLE Volume);