/**
 * This is the configration file for the screen display setting and such
 * These are resolved at compile time, as of now so its best to place them in a seprate file
 */

#pragma once

/**
 * @brief Target screen parameters
 * 
 */
#define TARGET_SCREEN_WIDTH 2048
#define TARGET_SCREEN_HEIGHT 1536

#define KERNEL_EXECUTABLE_PATH L"\\kernel.elf"
#define FONT_FILE_PATH L"\\zap-light16.psf"

/**
 * @brief The Debug Switch
 * 
 */
#define MAPPLE_DEBUG 1