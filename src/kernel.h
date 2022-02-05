#pragma once
#include "include/efi.h"
#include "include/efilib.h"

int kmain();
static inline void PlotPixel_32bpp(int x, int y, uint32_t pixel);