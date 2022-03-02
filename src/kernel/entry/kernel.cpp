#include <mapple/types.h>
#include <mapple/Memory.h>
#include <mapple/Gui.h>
#include <mapple/config.h>
#include <mapple/cstr.h>
#include <cstddef>

DebugConsole* GloballConsole;

void PrepareMemory(BootInfo_t* boot_info){
    DebugPrint("Preparing Memory...\n");
    
    uint64_t mMapEntries = boot_info->memoryMap.mMapSize / boot_info->memoryMap.descSize;

    PageFrameAllocator Allocator;

    GlobalAllocator = &Allocator;

    DebugPrint("Starting to read EfiMemoryMap\n");
    GlobalAllocator->ReadEFIMemoryMap(boot_info->memoryMap.mMap, boot_info->memoryMap.mMapSize, boot_info->memoryMap.descSize);
    DebugPrint("Done reading EfiMemoryMap\n");

    uint64_t kernelSize = (uint64_t)&kernel_start - (uint64_t)&kernel_end;
    uint64_t kernelPages = (uint64_t)kernelSize / 4096 + 1;
}

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

    DebugPrint("Done..\n");

    asm("hlt");

    return 0;
};