#include <mapple/types.h>
#include <mapple/Memory.h>
#include <mapple/Gui.h>
#include <mapple/config.h>
#include <mapple/cstr.h>
#include <cstddef>

extern "C" uint64_t kernel_main(
    BootInfo_t* boot_info
){
    GDTDescriptor DescTableItem;
    DescTableItem.Size = sizeof(GDT) - 1;
    DescTableItem.Offset = (uint64_t)&DefaultGDT;

    LoadGDT(&DescTableItem);

    clearScreen(boot_info->frameBuffer);

    DebugConsole console = DebugConsole(&boot_info->frameBuffer, &boot_info->defaultFont);
    console.Print("Print Service");
    
    asm("hlt");

    return 0;
};