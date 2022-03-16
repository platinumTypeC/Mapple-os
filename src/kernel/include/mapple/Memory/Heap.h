#pragma once
#include <mapple/types.h>

void InitializeHeap(void* HeapAddress, size_t PageCount);

void* Malloc(size_t Size);
void Free(void* Address);

void ExpandHeap(size_t Length);

struct HeapSegHdr{
    size_t Length;
    HeapSegHdr* Next;
    HeapSegHdr* Last;
    bool IsFree;
    void CombineForward();
    void CombineBackward();
    HeapSegHdr* Split(size_t SplitLength);

    void* operator new(size_t Size) {return Malloc(Size);}
    void* operator new[](size_t Size) {return Malloc(Size);}
    
    void operator delete(void* p) {Free(p);}
};

