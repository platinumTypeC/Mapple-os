#pragma once
#include <stdint.h>
#include <mapple/types.h>
#include <mapple/Bitmap.h>

class PageFrameAllocator {
    public:
    void ReadEFIMemoryMap(EFI_MEMORY_DESCRIPTOR* MemoryMapFirstDescriptor, size_t MemoryMapSize, size_t MemoryMapDescriptorSize);
    Bitmap_t PageBitmap;
    void FreePage(void* Address);
    void FreePages(void* Address, uint64_t PageCount);
    void LockPage(void* Address);
    void LockPages(void* Address, uint64_t PageCount);
    void* RequestPage();
    uint64_t GetFreeRAM();
    uint64_t GetUsedRAM();
    uint64_t GetReservedRAM();


    private:
    void InitBitmap(size_t BitmapSize, void* BufferAddress);
    void ReservePage(void* Address);
    void ReservePages(void* Address, uint64_t PageCount);
    void UnreservePage(void* Address);
    void UnreservePages(void* Address, uint64_t PageCount);
};

extern PageFrameAllocator* GlobalAllocator;