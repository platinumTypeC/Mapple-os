#pragma once

#include <stdint.h>
#include <mapple/paging/pageTableManager.h>

typedef struct {
    uint64_t Type;
    uint64_t Pad;
    uint64_t PhysicalStart;
    uint64_t VirtualStart;
    uint64_t NumberOfPages;
    uint64_t Attribute;
} EFI_MEMORY_DESCRIPTOR;


typedef struct {
	void* BaseAddress;
	uint64_t BufferSize;
	uint64_t Width;
	uint64_t Height;
	uint64_t PixelsPerScanLine;
} Framebuffer_t;

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04

typedef struct {
	unsigned char magic[2];
	unsigned char mode;
	unsigned char charsize;
} PSF1_HEADER;

struct PSF1_FONT{
	PSF1_HEADER* psf1_Header;
	void* glyphBuffer;
};

struct BootInfo_t{
	Framebuffer_t* framebuffer;
	PSF1_FONT* psf1_Font;
	EFI_MEMORY_DESCRIPTOR* mMap;
	uint64_t mMapSize;
	uint64_t mMapDescSize;
	void* rsdp;
};

struct KernelInfo_t {
    PageTableManager_t* KernelPageTableManager;
};

struct Point_t{
    uint64_t X;
    uint64_t Y;
};