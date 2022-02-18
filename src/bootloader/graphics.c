#include "include/graphics.h"

EFI_GRAPHICS_OUTPUT_PROTOCOL* graphics_output_protocol = NULL;
EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

/**
 * @brief Get the gop protocol object
 * 
 * @return EFI_GRAPHICS_OUTPUT_PROTOCOL* 
 */
EFI_GRAPHICS_OUTPUT_PROTOCOL* get_gop_protocol(){
    return graphics_output_protocol;
}
/**
 * @brief 
 * 
 * @param protocol 
 * @param _x 
 * @param _y 
 * @param width 
 * @param height 
 * @param color 
 * @return VOID 
 */
VOID draw_rect(IN EFI_GRAPHICS_OUTPUT_PROTOCOL* const protocol,
	IN const UINT16 _x,
	IN const UINT16 _y,
	IN const UINT16 width,
	IN const UINT16 height,
	IN const UINT32 color)
{
	/** Pointer to the current pixel in the buffer. */
	UINT32* at;

	for(UINT16 row = 0; row < height; row++) {
		for(UINT16 col = 0; col < width; col++) {
			at = (UINT32*)protocol->Mode->FrameBufferBase + _x + col;
			at += ((_y + row) * protocol->Mode->Info->HorizontalResolution);

			*at = color;
		}
	}
}

/**
 * @brief Finds a video mode.
 * Finds a particular video mode by its width, height and pixel format.
 * Tests all video modes copatible with the provided protocol, populating the
 * `video_mode` variable on success.
 * @param[in]     protocol The protocol to find the video mode in.
 * @param[in]     target_width The target width to search for.
 * @param[in]     target_height The target height to search for.
 * @param[in]     target_pixel_format The target pixel format to search for.
 * @param[out]    video_mode The video mode variable to populate.
 * @return The program status.
 * @retval EFI_SUCCESS    If the function executed successfully.
 * @retval other          Any other value is an EFI error code.
 */
EFI_STATUS find_video_mode(IN EFI_GRAPHICS_OUTPUT_PROTOCOL* const protocol,
	IN const UINT32 target_width,
	IN const UINT32 target_height,
	IN const EFI_GRAPHICS_PIXEL_FORMAT target_pixel_format,
	OUT UINTN* video_mode)
{
	/** The size of the video mode info struct. */
	UINTN size_of_mode_info;
	/** The video mode info struct. */
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* mode_info;

	UINTN i = 0;
	for(i = 0; i < protocol->Mode->MaxMode; i++) {
		#if MAPPLE_DEBUG != 0
			Print(L"Debug: Testing video mode: '%llu'\n", i);
		#endif

		CHECKER(
			uefi_call_wrapper(protocol->QueryMode, 4,
				protocol, i, &size_of_mode_info, &mode_info)
			,
			L"Error: Error querying video mode: %s\n"
		);

		if(mode_info->HorizontalResolution == target_width &&
			mode_info->VerticalResolution == target_height &&
			mode_info->PixelFormat == target_pixel_format) {

			#if MAPPLE_DEBUG != 0
				Print(L"Debug: Matched video mode: '%llu' for '%lu*%lu*%u'\n", i,
					target_width, target_height, target_pixel_format);
			#endif

			*video_mode = i;
			return EFI_SUCCESS;
		}
	}

	return EFI_UNSUPPORTED;
}

/**
 * @brief Set the graphics mode object
 * 
 * @param protocol 
 * @param target_width 
 * @param target_height 
 * @param target_pixel_format 
 * @return EFI_STATUS 
 */
EFI_STATUS set_graphics_mode(IN EFI_GRAPHICS_OUTPUT_PROTOCOL* const protocol,
	IN const UINT32 target_width,
	IN const UINT32 target_height,
	IN const EFI_GRAPHICS_PIXEL_FORMAT target_pixel_format)
{
	/** The graphics mode number. */
	UINTN graphics_mode_num = 0;

	CHECKER(
		find_video_mode(protocol, target_width, target_height,
			target_pixel_format, &graphics_mode_num)
		,
		L"Error: Unable to find video mode, error: %s\n\r"
	);

	CHECKER(
		uefi_call_wrapper(protocol->SetMode, 2,
			protocol, graphics_mode_num)
		,
		L"Error: Error setting graphics mode: %s\n"
	);

	return EFI_SUCCESS;
}


/**
 * @brief 
 * 
 * @param protocol 
 * @return VOID 
 */
VOID draw_test_screen(IN EFI_GRAPHICS_OUTPUT_PROTOCOL* const protocol)
{
	const UINT16 tile_width = protocol->Mode->Info->HorizontalResolution /
		TEST_SCREEN_COL_NUM;
	const UINT16 tile_height = protocol->Mode->Info->VerticalResolution /
		TEST_SCREEN_ROW_NUM;

	UINT8 p = 0;
	for(p = 0; p < TEST_SCREEN_TOTAL_TILES; p++) {
		UINT8 _x = p % TEST_SCREEN_COL_NUM;
		UINT8 _y = p / TEST_SCREEN_COL_NUM;

		UINT32 color = TEST_SCREEN_PRIMARY_COLOUR;
		if(((_y % 2) + _x) % 2) {
			color = TEST_SCREEN_SECONDARY_COLOUR;
		}

		draw_rect(protocol, tile_width * _x, tile_height * _y,
			tile_width, tile_height, color);
	}
}

/**
 * @brief init the graphics output service so that we can finally show something more meaning full 
 * on the screen along with Resizing it
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS init_graphics_output_service(EFI_HANDLE ImageHandle)
{
    
    CHECKER(
		uefi_call_wrapper(BS->LocateProtocol, 3, 
			&gopGuid, NULL, (void**)&graphics_output_protocol)
		,
		L"Error: Unable to locate GOP\n\r"
	);
#if MAPPLE_DEBUG != 0
    Print(L"Debug: Located the graphics output protocol");
#endif

    if (graphics_output_protocol){
        CHECKER(
			set_graphics_mode(graphics_output_protocol, 
				TARGET_SCREEN_WIDTH,TARGET_SCREEN_HEIGHT, PixelBlueGreenRedReserved8BitPerColor)
			,
			L"Fatal Error: Error seting graphics mode: %s\n"
		);
	}
#if MAPPLE_TEST_SCENE_ONE != 0
	// This is the tile drawing test
	draw_test_screen(graphics_output_protocol);
#endif
    
	return EFI_SUCCESS;
}

EFI_STATUS close_graphics(){

	Print(L"Debug: CLosing the graphics service\n\r");
	CHECKER(
		uefi_call_wrapper(gBS->FreePool, 1,
			graphics_output_protocol
		),
		
		L"Error: Error releasing GOP handle buffer: %s\n"
	);

	return EFI_SUCCESS;
};