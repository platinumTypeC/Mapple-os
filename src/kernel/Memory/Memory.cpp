#include <mapple/Memory.h>
#include <mapple/types.h>
#include <mapple/Gui.h>
#include <mapple/cstr.h>

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

const char* EFI_MEMORY_TYPE_STRINGS[] {

    "EfiReservedMemoryType",
    "EfiLoaderCode",
    "EfiLoaderData",
    "EfiBootServicesCode",
    "EfiBootServicesData",
    "EfiRuntimeServicesCode",
    "EfiRuntimeServicesData",
    "EfiConventionalMemory",
    "EfiUnusableMemory",
    "EfiACPIReclaimMemory",
    "EfiACPIMemoryNVS",
    "EfiMemoryMappedIO",
    "EfiMemoryMappedIOPortSpace",
    "EfiPalCode",
};

uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* MemoryMapFirstDescriptor, uint64_t MemoryMapEntries, uint64_t MemoryMapDescriptorSize){

    static uint64_t MemorySizeBytes = 0;
    if (MemorySizeBytes > 0) return MemorySizeBytes;

    for (uint64_t i = 0; i < MemoryMapEntries; i++){
        EFI_MEMORY_DESCRIPTOR* Descriptor = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)MemoryMapFirstDescriptor + (i * MemoryMapDescriptorSize));
        MemorySizeBytes += Descriptor->NumberOfPages * 4096;
    }

    return MemorySizeBytes;

}

void Memset(void* Start, uint8_t Value, uint64_t Num){
    for (uint64_t i = 0; i < Num; i++){
        *(uint8_t*)((uint64_t)Start + i) = Value;
    }
}
