#pragma once
#include <mapple/types.h>


class Bitmap_t{
    public:
    size_t Size;
    uint8_t* Buffer;
    bool operator[](uint64_t index){
        return Get(index);
    };
    bool Set(uint64_t index, bool value){
        if (index > Size * 8) return false;
        uint64_t byteIndex = index / 8;
        uint8_t bitIndex = index % 8;
        uint8_t bitIndexer = 0b10000000 >> bitIndex;
        Buffer[byteIndex] &= ~bitIndexer;
        if (value){
            Buffer[byteIndex] |= bitIndexer;
        }
        return true;
    };
    bool Get(uint64_t index){
        if (index > Size * 8) return false;
        uint64_t byteIndex = index / 8;
        uint8_t bitIndex = index % 8;
        uint8_t bitIndexer = 0b10000000 >> bitIndex;
        if ((Buffer[byteIndex] & bitIndexer) > 0){
            return true;
        }
        return false;
    };
};

#define Bitmap Bitmap_t

class PageFrameAllocator {
    public:
    void ReadEFIMemoryMap(EFI_MEMORY_DESCRIPTOR* mMap, size_t mMapSize, size_t mMapDescSize);
    Bitmap_t PageBitmap;
    void FreePage(void* address);
    void FreePages(void* address, uint64_t pageCount);
    void LockPage(void* address);
    void LockPages(void* address, uint64_t pageCount);
    void* RequestPage();
    uint64_t GetFreeRAM();
    uint64_t GetUsedRAM();
    uint64_t GetReservedRAM();


    private:
    void InitBitmap(size_t bitmapSize, void* bufferAddress);
    void ReservePage(void* address);
    void ReservePages(void* address, uint64_t pageCount);
    void UnreservePage(void* address);
    void UnreservePages(void* address, uint64_t pageCount);

};

extern PageFrameAllocator* GlobalAllocator;
