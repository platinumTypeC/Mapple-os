#include <mapple/Memory.h>
#include <mapple/types.h>
#include <mapple/Gui.h>

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