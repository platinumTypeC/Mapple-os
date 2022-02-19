#pragma once
#include "paging.h"

class PageTableManager_t {
    PageTableManager_t(PageTable* PML4Address);
    PageTable* PML4;
    void MapMemory(void* virtualMemory, void* physicalMemory);
};

extern PageTableManager_t GlobalPageTableManager;