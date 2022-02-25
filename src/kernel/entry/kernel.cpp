#include <mapple/types.h>
#include <mapple/Memory.h>
#include <mapple/Gui.h>
#include <cstddef>

extern "C" void kernel_main(
    BootInfo_t* boot_info
){
    // clearScreen(boot_info->frameBuffer);
    GDTDescriptor DescTableItem;
    DescTableItem.Size = sizeof(GDT) - 1;
    DescTableItem.Offset = (uint64_t)&DefaultGDT;

    LoadGDT(&DescTableItem);

    asm("hlt");
};