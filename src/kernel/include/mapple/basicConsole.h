#pragma once

#include <mapple/types.h>

class BasicRenderer_t{
    public:
    BasicRenderer_t(Framebuffer_t* NewTargetFramebuffer, PSF1_FONT* NewPSF1Font);
    Point_t CursorPosition;
    Framebuffer_t* TargetFramebuffer;
    PSF1_FONT* PSF1Font;
    uint32_t MouseCursorBuffer[16 * 16];
    uint32_t MouseCursorBufferAfter[16 * 16];
    unsigned int Colour;
    unsigned int ClearColour;
    void Print(const char* String);
    void PutChar(char Char, unsigned int XOffset, unsigned int YOffset);
    void PutChar(char Char);
    void PutPix(uint32_t X, uint32_t Y, uint32_t Colour);
    uint32_t GetPix(uint32_t X, uint32_t Y);
    void ClearChar();
    void Clear();
    void Next();
    void DrawOverlayMouseCursor(uint8_t* MouseCursor, Point_t Position, uint32_t Colour);
    void ClearMouseCursor(uint8_t* MouseCursor, Point_t Position);
    bool MouseDrawn;
};