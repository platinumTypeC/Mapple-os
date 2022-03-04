#pragma once
#include <mapple/Memory/Paging/Paging.h>
#include <mapple/Memory.h>

class PageTableManager {
    public:
    // struct is required here for some reason
    struct PageTable* PML4;
    PageTableManager(struct PageTable* PML4Address);
    void MapMemory(void* virtualMemory, void* physicalMemory);
};

extern PageTableManager* GlobalPageTableManager;