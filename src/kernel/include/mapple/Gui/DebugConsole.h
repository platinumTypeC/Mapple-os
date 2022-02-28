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
        void Print(const char* str);
        void Print(const char* str, uint64_t formatInt);
        void putPix(uint64_t X, uint64_t Y, uint64_t Color);
        void PutChar(char Char, unsigned int XOffset, unsigned int YOffset);
};

#if MAPPLE_DEBUG != 0
    #define DebugPrint(console,...) console.Print(__VA_ARGS__)
#else
    #define DebugPrint(console,...)
#endif