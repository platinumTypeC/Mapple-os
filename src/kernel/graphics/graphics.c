#include <mapple/graphics.h>
#include <mapple/config.h>

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

void draw_test_scene(
	uint64_t FrameBufferBaseAddress,
	uint64_t HorizontalResolution,
	uint64_t VerticalResolution
)
{
	const uint16_t tile_width = HorizontalResolution / TEST_SCREEN_COL_NUM;
	const uint16_t tile_height = VerticalResolution / TEST_SCREEN_ROW_NUM;

	uint8_t p = 0;
	for(p = 0; p < TEST_SCREEN_TOTAL_TILES; p++) {
		uint8_t _x = p % TEST_SCREEN_COL_NUM;
		uint8_t _y = p / TEST_SCREEN_COL_NUM;

		uint32_t color = 0x00202363;
		if(((_y % 2) + _x) % 2) {
			color = 0x00FFB41F;
		}

		draw_rect(tile_width * _x, tile_height * _y, tile_width, tile_height, color, FrameBufferBaseAddress, HorizontalResolution);
	}
}