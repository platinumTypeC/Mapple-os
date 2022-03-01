#pragma once
#include <mapple/Memory/GDT.h>
#include <mapple/Memory/Paging.h>
#include <mapple/Memory/PageFameAllocator.h>
#include <mapple/types.h>

void PrepareMemory(BootInfo_t* boot_info);