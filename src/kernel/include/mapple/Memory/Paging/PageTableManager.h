#pragma once
#include <mapple/Memory/Paging/Paging.h>

class PageTableManager {
    public:
    // struct is required here for some reason
    struct PageTable* PML4;
    PageTableManager(struct PageTable* PML4Address);
    void MapMemory(void* virtualMemory, void* physicalMemory, uint64_t __TEST_VAL = 0);
};

extern PageTableManager* GlobalPageTableManager;
