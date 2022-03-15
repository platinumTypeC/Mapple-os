#pragma once
#include <mapple/Memory/GDT.h>
#include <mapple/Memory/Heap.h>
#include <mapple/Memory/Paging/Paging.h>
#include <mapple/types.h>

void PrepareMemory(BootInfo_t* bootInfo);
uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* MemoryMapFirstDescriptor, uint64_t MemoryMapEntries, uint64_t MemoryMapDescriptorSize);
void Memset(void* Start, uint8_t Value, uint64_t Num);
