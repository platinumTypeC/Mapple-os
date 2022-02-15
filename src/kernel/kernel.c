#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "bootInfo.h"
#include "graphics/graphics.h"


void kernel_main(Boot_Info* boot_info)
{
	const uint16_t tile_width = boot_info->HorizontalResolution / 4;
	const uint16_t tile_height = boot_info->VerticalResolution / 3;

	uint8_t p = 0;
	for(p = 0; p < 12; p++) {
		uint8_t _x = p % 4;
		uint8_t _y = p / 4;

		uint32_t color = 0x00202363;
		if(((_y % 2) + _x) % 2) {
			color = 0x00FFB41F;
		}

		draw_rect(tile_width * _x, tile_height * _y, tile_width, tile_height, color, boot_info->FrameBufferBaseAddress, boot_info->HorizontalResolution);
	}


	while (1);
}