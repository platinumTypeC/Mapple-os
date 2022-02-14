#include <stdbool.h>
#include <stddef.h>
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
	uint64_t FrameBufferAddr;
	uint64_t mmap_size;
	uint64_t mmap_descriptor_size;
} Boot_Info;

void kernel_main(Boot_Info* boot_info)
{
	while (1);
}