#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <mapple/bootInfo.h>
#include <mapple/graphics.h>


void kernel_main(Boot_Info* boot_info)
{
	draw_test_scene(
		boot_info->FrameBufferBaseAddress,
		boot_info->HorizontalResolution,
		boot_info->VerticalResolution
	);
	while (1);
}