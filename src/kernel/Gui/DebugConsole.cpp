#include <mapple/Gui.h>

DebugConsole::DebugConsole(Framebuffer_t* frmaebuffer, PSF1_FONT_t* NewPSF1Font){
    TargetFramebuffer = frmaebuffer;
    PSF1Font = NewPSF1Font;
    Color = 0xffffffff;
    CursorPosition = {10, 10};
};

// void DebugConsole::Print(const char* str){    
// };

void DebugConsole::putPix(uint64_t X, uint64_t Y, uint64_t Color){
    *(uint32_t*)((uint64_t)TargetFramebuffer->BaseAddress + (X*4) + (Y * TargetFramebuffer->PixelsPerScanLine * 4)) = Color;
};