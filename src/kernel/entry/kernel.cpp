#include <mapple/types.h>
#include <mapple/basicRenderer.h>
#include <mapple/global.h>

extern "C" void kernel_main(
    BootInfo_t* boot_info
){
    
    while (true)
    {
        asm("hlt");
    }  
};