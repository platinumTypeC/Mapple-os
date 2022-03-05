#include <mapple/types.h>
#include <mapple/Memory.h>
#include <mapple/Gui.h>
#include <mapple/config.h>
#include <mapple/cstr.h>
#include <cstddef>

DebugConsole* GloballConsole;
Framebuffer_t* GlobalFrameBuffer;
PageTableManager* GlobalPageTableManager;

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

void PrepareMemory(BootInfo_t* bootInfo){
    DebugPrint("Preparing Memory\n");

    GDTDescriptor gdtDescriptor;
    gdtDescriptor.Size = sizeof(GDT) - 1;
    gdtDescriptor.Offset = (uint64_t)&DefaultGDT;
    LoadGDT(&gdtDescriptor);

    DebugPrint("GDTdescriptor Initialized \n");

    uint64_t mMapEntries = bootInfo->memoryMap.mMapSize / bootInfo->memoryMap.descSize;

    PageFrameAllocator allocator;
    GlobalAllocator = &allocator;
    GlobalAllocator->ReadEFIMemoryMap(bootInfo->memoryMap.mMap, bootInfo->memoryMap.mMapSize, bootInfo->memoryMap.descSize);
    
    uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    uint64_t kernelPages = (uint64_t)kernelSize / 4096 + 1;

    DebugPrint("Locking Pages: ");
    DebugPrint(to_hstring(kernelPages));
    DebugPrint("\n");

    GlobalAllocator->LockPages(&_KernelStart, kernelPages);

    DebugPrint("Free Ram: ");
    DebugPrint(to_string(GlobalAllocator->GetFreeRAM() / 1024));
    DebugPrint(" KB\n");

    DebugPrint("Used Ram: ");
    DebugPrint(to_string(GlobalAllocator->GetUsedRAM() / 1024));
    DebugPrint(" KB\n");

    DebugPrint("Reserved Ram: ");
    DebugPrint(to_string(GlobalAllocator->GetReservedRAM() / 1024));
    DebugPrint(" KB\n");

    PageTable* PML4 = (PageTable*)GlobalAllocator->RequestPage();
    Memset(PML4, 0, 0x1000);

    PageTableManager pageManager(PML4);
    GlobalPageTableManager = &pageManager;

    for (uint64_t t = 0; t < GetMemorySize(bootInfo->memoryMap.mMap, mMapEntries, bootInfo->memoryMap.descSize); t+= 0x1000){
        GlobalPageTableManager->MapMemory((void*)t, (void*)t);
    };

    GlobalAllocator->LockPages((void*)GlobalFrameBuffer->BaseAddress, GlobalFrameBuffer->BufferSize / 0x1000 + 1);

    for (uint64_t t = GlobalFrameBuffer->BaseAddress; t < GlobalFrameBuffer->BaseAddress + GlobalFrameBuffer->BufferSize; t += 4096){
        GlobalPageTableManager->MapMemory((void*)t, (void*)t);
    }
    DebugPrint("Locked Frame Buffer Pages.\n");

    asm ("mov %0, %%cr3" : : "r" (PML4));
    DebugPrint("Prepared Memory\n");
}

extern "C" uint64_t kernel_main(
    BootInfo_t* boot_info
){
    GlobalFrameBuffer = &boot_info->frameBuffer;
    clearScreen();

    DebugConsole console = DebugConsole(&boot_info->defaultFont);
    GloballConsole = &console;
    DebugPrint("Starting Initialization\n");

    PrepareMemory(boot_info);

    DebugPrint("Done.. Haulting\n");
    asm("hlt");

    return 0;
};