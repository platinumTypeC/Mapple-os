#include "graphics.h"

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