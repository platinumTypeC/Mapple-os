#pragma once
#include <mapple/types.h>
#include <mapple/config.h>
#include <mapple/cstr.h>

class DebugConsole
{
    public:
        Framebuffer_t* TargetFramebuffer;
        PSF1_FONT_t* PSF1Font;
        uint32_t Color;
        const uint64_t ClearColor = 0x00000000;
        Point_t CursorPosition;
        bool MouseDrawn;
        uint32_t MouseCursorBuffer[16 * 16];
        uint32_t MouseCursorBufferAfter[16 * 16];
        
        DebugConsole(PSF1_FONT_t* NewPSF1Font);
        void Print(const char* str);
        void ClearChar();
        void PutPix(uint64_t X, uint64_t Y, uint64_t Color);
        void PutChar(char Char, unsigned int XOffset, unsigned int YOffset);
        void DrawOverlayMouseCursor(uint8_t* MouseCursor, Point Position, uint32_t Colour);
        void ClearMouseCursor(uint8_t* MouseCursor, Point Position);
        uint32_t GetPix(uint64_t X, uint64_t Y);
};

extern DebugConsole* GloballConsole;

#if MAPPLE_DEBUG != 0
    #define DebugPrint(...) GloballConsole->Print(__VA_ARGS__)
    #define DebugPrintNum(...) GloballConsole->Print(to_string(__VA_ARGS__))
#else
    #define DebugPrint(...)
#endif
