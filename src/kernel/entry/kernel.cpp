#include <mapple/types.h>
#include <mapple/types.hpp>
#include <mapple/Memory.h>
#include <cstddef>

extern "C" void kernel_main(
    BootInfo_t* boot_info
){
    {
        BasicRenderer_t renderer = BasicRenderer_t(boot_info->frameBuffer, &boot_info->defaultFont);
        SetGlobalRenderer(&renderer);
    }

    GDTDescriptor DescTableItem;
    DescTableItem.Size = sizeof(GDT) - 1;
    DescTableItem.Offset = (uint64_t)&DefaultGDT;

    LoadGDT(&DescTableItem);

    asm("hlt");
};