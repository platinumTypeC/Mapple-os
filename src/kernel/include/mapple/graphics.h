#pragma once

#include <stdint.h>

void draw_rect(
    uint16_t _x,
    uint16_t _y,
    uint16_t width,
    uint16_t height,
    uint32_t color,
    uint64_t FrameBufferBase,
    uint64_t HorizontalResolution
);

void draw_test_scene(
	uint64_t FrameBufferBaseAddress,
	uint64_t HorizontalResolution,
	uint64_t VerticalResolution
);