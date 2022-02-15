#include <mapple/shedular.h>
#include <mapple/graphics.h>

void runner( void (*function)() ){
    function();
};

void shedular(Boot_Info* boot_info) {
    draw_test_scene(
        boot_info->FrameBufferBaseAddress,
        boot_info->HorizontalResolution,
        boot_info->VerticalResolution
    );
};