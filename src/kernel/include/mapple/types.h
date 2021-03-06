#pragma once
#include <stddef.h>

#ifdef NO_LL
#include <stdint.h>
#else
#include <mapple/types/linkedList.h>
#endif

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04
#define EFI_MEMORY_DESCRIPTOR efi_memoryDescrupter_t

typedef struct {
    uint32_t Type;           // Field size is 32 bits followed by 32 bit pad
    uint32_t Pad;
    uint64_t PhysicalStart;  // Field size is 64 bits
    uint64_t VirtualStart;   // Field size is 64 bits
    uint64_t NumberOfPages;  // Field size is 64 bits
    uint64_t Attribute;      // Field size is 64 bits
} EFI_MEMORY_DESCRIPTOR;

typedef struct {
	uint8_t magic[2];
	uint8_t mode;
	uint8_t charsize;
} PSF1_HEADER_t;


typedef struct {
    PSF1_HEADER_t* psf1_Header;
    void* glyphBuffer;
} PSF1_FONT_t;

typedef struct {
	uint64_t BaseAddress;
	uint64_t BufferSize;
	uint64_t Width;
	uint64_t Height;
	uint64_t PixelsPerScanLine;
} Framebuffer_t;

typedef struct
{
    efi_memoryDescrupter_t* mMap;
    uint64_t mMapSize;
    uint64_t mMapKey;
    uint32_t descVersion;
    uint64_t descSize;
} MemoryInfo_t;

typedef struct {
    Framebuffer_t frameBuffer;
    PSF1_FONT_t defaultFont;
    MemoryInfo_t memoryMap;
    void* rsdp;
} BootInfo_t;

typedef struct
{
    uint64_t X;
    uint64_t Y;
} Point_t;

typedef struct
{
    uint32_t r;
    uint32_t g;
    uint32_t b;
    uint32_t a;
} Color_t;

extern const char* EFI_MEMORY_TYPE_STRINGS[];

#define Point Point_t
#define BootInfo BootInfo_t
#define MemoryInfo MemoryInfo_t
#define Framebuffer Framebuffer_t
#define PSF1_FONT PSF1_FONT_t
#define PSF1_HEADER PSF1_HEADER_t
#define BMP_IMAGE_HEADER BMP_IMAGE_HEADER_t

#define align_up(num, align) (((num) + ((align)-1)) & ~((align)-1))
