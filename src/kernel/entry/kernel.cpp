#include <mapple/types.h>
#include <mapple/Memory.h>
#include <mapple/Gui.h>
#include <mapple/config.h>
#include <mapple/cstr.h>
#include <cstddef>

DebugConsole* GloballConsole;

extern "C" uint64_t kernel_main(
    BootInfo_t* boot_info
){
    clearScreen(boot_info->frameBuffer);

    DebugConsole console = DebugConsole(&boot_info->frameBuffer, &boot_info->defaultFont);
    GloballConsole = &console;
    DebugPrint("\n");

    GDTDescriptor DescTableItem;
    DescTableItem.Size = sizeof(GDT) - 1;
    DescTableItem.Offset = (uint64_t)&DefaultGDT;

    LoadGDT(&DescTableItem);

    PrepareMemory(boot_info);

    return 0;
};