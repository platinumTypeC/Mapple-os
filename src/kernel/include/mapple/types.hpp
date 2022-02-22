#pragma once

#include <stdint.h>
#include <mapple/types.h>

class BasicRenderer{
    public:
    BasicRenderer(Framebuffer* NewTargetFramebuffer, PSF1_FONT* NewPSF1Font);
    Point_t CursorPosition;
    Framebuffer_t* TargetFramebuffer;
    PSF1_FONT_t* PSF1Font;
    uint32_t MouseCursorBuffer[16 * 16];
    uint32_t MouseCursorBufferAfter[16 * 16];
    uint64_t Colour;
    uint64_t ClearColour = 0;
    void Print(const char* String);
    void PutChar(char Char, unsigned int XOffset, unsigned int YOffset);
    void PutChar(char Char);
    void PutPix(uint32_t X, uint32_t Y, uint32_t Colour);
    uint32_t GetPix(uint32_t X, uint32_t Y);
    void ClearChar();
    void Clear();
    void Next();
    void DrawOverlayMouseCursor(uint8_t* MouseCursor, Point Position, uint32_t Colour);
    void ClearMouseCursor(uint8_t* MouseCursor, Point Position);
    bool MouseDrawn;
};

#define BasicRenderer_t BasicRenderer

BasicRenderer_t* GetGlobalRenderer();
void SetGlobalRenderer(BasicRenderer_t* setter);