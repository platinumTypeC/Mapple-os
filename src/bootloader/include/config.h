/**
 * This is the configration file for the screen display setting and such
 * These are resolved at compile time, as of now so its best to place them in a seprate file
 */

#pragma once

#include <efi.h>
#include <efilib.h>

#define MAPPLE_TEST_SCENE_ONE 1

#define TARGET_SCREEN_WIDTH 1024
#define TARGET_SCREEN_HEIGHT 768
#define TARGET_PIXEL_FORMAT PixelBlueGreenRedReserved8BitPerColor

#define TEST_SCREEN_COL_NUM             4
#define TEST_SCREEN_ROW_NUM             3
#define TEST_SCREEN_TOTAL_TILES         TEST_SCREEN_COL_NUM * TEST_SCREEN_ROW_NUM
#define TEST_SCREEN_PRIMARY_COLOUR      0x00FF4000
#define TEST_SCREEN_SECONDARY_COLOUR    0x00FF80BF