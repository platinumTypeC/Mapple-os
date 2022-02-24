#include <mapple/types.h>
#include <mapple/Memory.h>
#include <mapple/Gui/common.h>
#include <cstddef>

extern "C" void kernel_main(
    BootInfo_t* boot_info
){
    testScene(boot_info->frameBuffer);

    GDTDescriptor DescTableItem;
    DescTableItem.Size = sizeof(GDT) - 1;
    DescTableItem.Offset = (uint64_t)&DefaultGDT;

    LoadGDT(&DescTableItem);

    asm("hlt");
};