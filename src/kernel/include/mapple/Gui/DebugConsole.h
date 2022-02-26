#pragma once
#include <mapple/types.h>

class DebugConsole
{
    public:
        Framebuffer_t* TargetFramebuffer;
        PSF1_FONT_t* PSF1Font;
        uint32_t Color;
        Point_t CursorPosition;
        
        DebugConsole(Framebuffer_t* frmaebuffer, PSF1_FONT_t* NewPSF1Font);
        // void Print(const char* str); Working on it
        void PutChar(char chr, unsigned int xOff, unsigned int yOff);
        void putPix(uint64_t X, uint64_t Y, uint64_t Color);
};