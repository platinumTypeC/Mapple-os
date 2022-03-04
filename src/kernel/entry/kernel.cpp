#include <mapple/types.h>
#include <mapple/Memory.h>
#include <mapple/Gui.h>
#include <mapple/config.h>
#include <mapple/cstr.h>
#include <cstddef>

DebugConsole* GloballConsole;
Framebuffer_t* GlobalFrameBuffer;
PageTableManager* GlobalPageTableManager;

struct KernelInfo {
    PageTableManager* pageTableManager;
};

KernelInfo kernelInfo;

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

    // Tooooooooooo Slooooooooooow for testing
    // uint64_t kernelSize = (uint64_t)&kernel_start - (uint64_t)&kernel_end;
    // uint64_t kernelPages = ((uint64_t)kernelSize / 4096 + 1);
    uint64_t kernelPages = 0xFFFFF4;

    DebugPrint("Locking Pages: ");
    DebugPrint(to_hstring(kernelPages));
    DebugPrint("\n");

    GlobalAllocator->LockPages(&kernel_start, kernelPages);

    PageTable* PML4 = (PageTable*)GlobalAllocator->RequestPage();
    Memset(PML4, 0, 0x1000);

    PageTableManager tableManager(PML4);

    GlobalPageTableManager = &tableManager;

    for (uint64_t t = 0; t < GetMemorySize(bootInfo->memoryMap.mMap, mMapEntries, bootInfo->memoryMap.descSize); t+= 0x1000){
        GlobalPageTableManager->MapMemory((void*)t, (void*)t);
    }

    uint64_t fbBase = (uint64_t)bootInfo->frameBuffer.BaseAddress;
    uint64_t fbSize = (uint64_t)bootInfo->frameBuffer.BufferSize + 0x1000;
    GlobalAllocator->LockPages((void*)fbBase, fbSize/ 0x1000 + 1);
    for (uint64_t t = fbBase; t < fbBase + fbSize; t += 4096){
        GlobalPageTableManager->MapMemory((void*)t, (void*)t);
    }

    asm ("mov %0, %%cr3" : : "r" (PML4));

    kernelInfo.pageTableManager = GlobalPageTableManager;
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

    DebugPrint("Done Preparing Memory\n");

    DebugPrint("Done.. Haulting\n");

    asm("hlt");

    return 0;
};