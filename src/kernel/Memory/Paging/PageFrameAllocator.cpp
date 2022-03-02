#include <mapple/Memory.h>
#include <mapple/Gui.h>
#include <mapple/cstr.h>

uint64_t FreeMemory;
uint64_t ReservedMemory;
uint64_t UsedMemory;
bool Initialized = false;
PageFrameAllocator* GlobalAllocator;

void PageFrameAllocator::ReadEFIMemoryMap(EFI_MEMORY_DESCRIPTOR* MemoryMapFirstDescriptor, size_t MemoryMapSize, size_t MemoryMapDescriptorSize){
    if (Initialized) return;

    Initialized = true;

    uint64_t MemoryMapEntries = MemoryMapSize / MemoryMapDescriptorSize;

    void* LargestFreeMemorySegment = NULL;
    size_t LargestFreeMemorySegmentSize = 0;

    for (uint64_t i = 0; i < MemoryMapEntries; i++){
        EFI_MEMORY_DESCRIPTOR* Descriptor = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)MemoryMapFirstDescriptor + (i * MemoryMapDescriptorSize));
        if (Descriptor->Type == 7){ // type = EfiConventionalMemory
            if (Descriptor->NumberOfPages * 4096 > LargestFreeMemorySegmentSize)
            {
                LargestFreeMemorySegment = (void*)Descriptor->PhysicalStart;
                LargestFreeMemorySegmentSize = Descriptor->NumberOfPages * 4096;
            }
        }
    }

    uint64_t MemorySize = GetMemorySize(MemoryMapFirstDescriptor, MemoryMapEntries, MemoryMapDescriptorSize);
    FreeMemory = MemorySize;
    uint64_t BitmapSize = MemorySize / 4096 / 8 + 1;

    InitBitmap(BitmapSize, LargestFreeMemorySegment);

    ReservePages(0, MemorySize / 4096 + 1);
    for (uint64_t i = 0; i < MemoryMapEntries; i++){
        EFI_MEMORY_DESCRIPTOR* Descriptor = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)MemoryMapFirstDescriptor + (i * MemoryMapDescriptorSize));
        if (Descriptor->Type == 7){ // efiConventionalMemory
            UnreservePages((void*)Descriptor->PhysicalStart, Descriptor->NumberOfPages);
        }
    }
    ReservePages(0, 0x100); // reserve between 0 and 0x100000
    LockPages(PageBitmap.Buffer, PageBitmap.Size / 4096 + 1);
}

void PageFrameAllocator::InitBitmap(size_t BitmapSize, void* BufferAddress){
    PageBitmap.Size = BitmapSize;
    PageBitmap.Buffer = (uint8_t*)BufferAddress;
    for (uint64_t i = 0; i < BitmapSize; i++){
        *(uint8_t*)(PageBitmap.Buffer + i) = 0;
    }
}
uint64_t PageBitmapIndex = 0;
void* PageFrameAllocator::RequestPage(){
    for (; PageBitmapIndex < PageBitmap.Size * 8; PageBitmapIndex++){
        if (PageBitmap[PageBitmapIndex] == true) continue;
        LockPage((void*)(PageBitmapIndex * 4096));
        return (void*)(PageBitmapIndex * 4096);
    }

    return NULL; // Page Frame Swap to file
}

void PageFrameAllocator::FreePage(void* Address){
    uint64_t Index = (uint64_t)Address / 4096;
    if (PageBitmap[Index] == false) return;
    if (PageBitmap.Set(Index, false)){
        FreeMemory += 4096;
        UsedMemory -= 4096;
        if (PageBitmapIndex > Index) PageBitmapIndex = Index;
    }
}

void PageFrameAllocator::FreePages(void* Address, uint64_t PageCount){
    for (uint64_t t = 0; t < PageCount; t++){
        FreePage((void*)((uint64_t)Address + (t * 4096)));
    }
}

void PageFrameAllocator::LockPage(void* Address){
    uint64_t Index = (uint64_t)Address / 4096;
    if (PageBitmap[Index] == true) return;
    if (PageBitmap.Set(Index, true)){
        FreeMemory -= 4096;
        UsedMemory += 4096;
    }
}

void PageFrameAllocator::LockPages(void* Address, uint64_t PageCount){
    DebugPrint("Locking number of pages..");
    DebugPrint(to_hstring(PageCount));
    DebugPrint("\n");
    for (uint64_t t = 0; t < PageCount; t++){
        LockPage((void*)((uint64_t)Address + (t * 4096)));
    }
}

void PageFrameAllocator::UnreservePage(void* Address){
    uint64_t Index = (uint64_t)Address / 4096;
    if (PageBitmap[Index] == false) return;
    if (PageBitmap.Set(Index, false)){
        FreeMemory += 4096;
        ReservedMemory -= 4096;
        if (PageBitmapIndex > Index) PageBitmapIndex = Index;
    }
}

void PageFrameAllocator::UnreservePages(void* Address, uint64_t PageCount){
    for (uint64_t t = 0; t < PageCount; t++){
        UnreservePage((void*)((uint64_t)Address + (t * 4096)));
    }
}

void PageFrameAllocator::ReservePage(void* Address){
    uint64_t Index = (uint64_t)Address / 4096;
    if (PageBitmap[Index] == true) return;
    if (PageBitmap.Set(Index, true)){
        FreeMemory -= 4096;
        ReservedMemory += 4096;
    }
}

void PageFrameAllocator::ReservePages(void* Address, uint64_t PageCount){
    for (uint64_t t = 0; t < PageCount; t++){
        ReservePage((void*)((uint64_t)Address + (t * 4096)));
    }
}

uint64_t PageFrameAllocator::GetFreeRAM(){
    return FreeMemory;
}
uint64_t PageFrameAllocator::GetUsedRAM(){
    return UsedMemory;
}
uint64_t PageFrameAllocator::GetReservedRAM(){
    return ReservedMemory;
}