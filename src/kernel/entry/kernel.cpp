#include <mapple/types.h>
#include <mapple/utils.h>

extern "C" void kernel_main(
    BootInfo_t* boot_info
){
    while (true)
    {
        asm("hlt");
    }  
};