#include <mapple/Gui/common.h>
#include <mapple/config.h>
#include <stddef.h>

void draw_rect(
    Framebuffer_t frameBuffer,
    uint16_t _x,
    uint16_t _y,
    uint16_t width,
    uint16_t height,
    uint32_t color
){
	/** Pointer to the current pixel in the buffer. */
	uint32_t* at;

	uint16_t row = 0;
	uint16_t col = 0;
	for(row = 0; row < height; row++) {
		for(col = 0; col < width; col++) {
			at = (uint32_t*)frameBuffer.BaseAddress + _x + col;
			at += ((_y + row) * frameBuffer.Width);
			*at = color;
		}
	}
}

void testScene(
    Framebuffer_t frameBuffer
){
    uint16_t tile_width = frameBuffer.Width /
		4;
    uint16_t tile_height = frameBuffer.Height /
		3;

	uint8_t p = 0;
	for(p = 0; p < 12; p++) {
		uint8_t _x = p % 4;
		uint8_t _y = p / 4;

		uint32_t color = TEST_SCREEN_PRIMARY_COLOUR;
		if(((_y % 2) + _x) % 2) {
			color = TEST_SCREEN_SECONDARY_COLOUR;
		}

		draw_rect(frameBuffer, tile_width * _x, tile_height * _y, tile_width, tile_height, color);
	}
};

void clearScreen(
	Framebuffer_t frameBuffer
){
	uint8_t _x = 0;
	uint8_t _y = 0;

	uint32_t color = CLEAR_SCREEN_COLOR;

	draw_rect(frameBuffer, frameBuffer.Width * _x, frameBuffer.Height * _y, frameBuffer.Width, frameBuffer.Height, color);
};