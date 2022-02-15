#pragma once
#include <stdint.h>

typedef struct s_memory_region_desc {
	uint32_t type;
	uintptr_t physical_start;
	uintptr_t virtual_start;
	uint64_t count;
	uint64_t attributes;
} Memory_Map_Descriptor;

typedef struct s_boot_info {
	Memory_Map_Descriptor* memory_map;
	uint64_t memory_map_size;
	uint64_t memory_map_descriptor_size;
	uint64_t FrameBufferBaseAddress;
	uint64_t HorizontalResolution;
	uint64_t VerticalResolution;
	uint64_t PixelsPerScanLine;
} Boot_Info;
