#pragma once

#include <mapple/bootInfo.h>
#include <mapple/config.h>
#include <stdint.h>

void init_graphics(
	BootInfo_t* boot_info
);

void debug_patern(
    uint64_t firstColor,
    uint64_t secondColor
);