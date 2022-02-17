#include <mapple/graphics.h>

uint64_t FrameBufferBaseAddress;
uint64_t HorizontalResolution;
uint64_t VerticalResolution;

void init_graphics(
	Boot_Info* boot_info
){
	FrameBufferBaseAddress = boot_info->FrameBufferBaseAddress;
	HorizontalResolution = boot_info->HorizontalResolution;
	VerticalResolution = boot_info->VerticalResolution;
	// Best Way to communicate debug info as of right now,
	// I will need to figure out some graphics later
	debug_patern(
		0x00202363,
		0x00FFB41F
	);
};

void draw_rect(
    uint16_t _x,
    uint16_t _y,
    uint16_t width,
    uint16_t height,
    uint32_t color,
    uint64_t FrameBufferBase,
    uint64_t HorizontalResolution
){
	/** Pointer to the current pixel in the buffer. */
	uint32_t* at;

	for(uint16_t row = 0; row < height; row++) {
		for(uint16_t col = 0; col < width; col++) {
			at = (uint32_t*)FrameBufferBase + _x + col;
			at += ((_y + row) * HorizontalResolution);

			*at = color;
		}
	}
}


/**
 * @brief 
 * 
 * @param firstColor 0x00202363
 * @param secondColor 0x00FFB41F
 */
void debug_patern(
    uint64_t firstColor,
    uint64_t secondColor
)
{
	const uint16_t tile_width = HorizontalResolution / TEST_SCREEN_COL_NUM;
	const uint16_t tile_height = VerticalResolution / TEST_SCREEN_ROW_NUM;

	uint8_t p = 0;
	for(p = 0; p < TEST_SCREEN_TOTAL_TILES; p++) {
		uint8_t _x = p % TEST_SCREEN_COL_NUM;
		uint8_t _y = p / TEST_SCREEN_COL_NUM;

		uint32_t color = firstColor;
		if(((_y % 2) + _x) % 2) {
			color = secondColor;
		}

		draw_rect(tile_width * _x, tile_height * _y, tile_width, tile_height, color, FrameBufferBaseAddress, HorizontalResolution);
	}
};