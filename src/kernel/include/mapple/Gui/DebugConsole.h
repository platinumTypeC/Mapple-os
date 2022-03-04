#pragma once
#include <mapple/types.h>
#include <mapple/config.h>

class DebugConsole
{
    public:
        Framebuffer_t* TargetFramebuffer;
        PSF1_FONT_t* PSF1Font;
        uint32_t Color;
        Point_t CursorPosition;
        
        DebugConsole(PSF1_FONT_t* NewPSF1Font);
        void Print(const char* str);
        void putPix(uint64_t X, uint64_t Y, uint64_t Color);
        void PutChar(char Char, unsigned int XOffset, unsigned int YOffset);
};

extern DebugConsole* GloballConsole;

#if MAPPLE_DEBUG != 0
    #define DebugPrint(...) GloballConsole->Print(__VA_ARGS__)
#else
    #define DebugPrint(...)
#endif
