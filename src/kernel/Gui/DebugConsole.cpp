#include <mapple/Gui.h>
#include <mapple/cstr.h>

DebugConsole::DebugConsole(PSF1_FONT_t* NewPSF1Font){
    TargetFramebuffer = GlobalFrameBuffer;
    PSF1Font = NewPSF1Font;
    Color = 0xffffffff;
    CursorPosition = {10, 10};
};

void DebugConsole::PutChar(char Char, uint32_t XOffset, uint32_t YOffset) {
    char* fontPtr = (char*)PSF1Font->glyphBuffer + (Char * PSF1Font->psf1_Header->charsize);
    for(unsigned long Y = YOffset; Y < YOffset+16; Y++) {
        for(unsigned long X = XOffset; X < XOffset+8; X++) {
            if ((*fontPtr & (0b10000000 >> (X - XOffset))) > 0){
                putPix(X, Y, Color);
            }
        }
        fontPtr++;
    }
}

void DebugConsole::Print(const char* str){
    char* fmt = (char*)str;
    while (*fmt != 0) {
        if (*fmt == '\n'){
            if (CursorPosition.Y + 16 > TargetFramebuffer->Height ){
                clearScreen();
                CursorPosition.Y = 10;
                CursorPosition.X = 10;
            }
            CursorPosition.X = 10;
            CursorPosition.Y += 16;
        }
        PutChar(*fmt, CursorPosition.X, CursorPosition.Y);
        CursorPosition.X += 8;
        if (CursorPosition.X +8 > TargetFramebuffer->Width) {
            // Temp Solution
            if (CursorPosition.Y + 16 > TargetFramebuffer->Height ){
                clearScreen();
            }
            CursorPosition.X = 10;
            CursorPosition.Y += 16;
        }
        fmt++;
    }
};

void DebugConsole::putPix(uint64_t X, uint64_t Y, uint64_t Color){
    *(uint32_t*)((uint64_t)TargetFramebuffer->BaseAddress + (X*4) + (Y * TargetFramebuffer->PixelsPerScanLine * 4)) = Color;
};
