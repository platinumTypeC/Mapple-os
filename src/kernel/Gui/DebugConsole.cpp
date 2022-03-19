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
                PutPix(X, Y, Color);
            }
        }
        fontPtr++;
    }
}

uint32_t DebugConsole::GetPix(uint64_t X, uint64_t Y){
    return *(uint32_t*)((uint64_t)TargetFramebuffer->BaseAddress + (X*4) + (Y * TargetFramebuffer->PixelsPerScanLine * 4));
};

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

void DebugConsole::PutPix(uint64_t X, uint64_t Y, uint64_t Color){
    *(uint32_t*)((uint64_t)TargetFramebuffer->BaseAddress + (X*4) + (Y * TargetFramebuffer->PixelsPerScanLine * 4)) = Color;
};

void DebugConsole::ClearMouseCursor(uint8_t* MouseCursor, Point Position){
    if (!MouseDrawn) return;

    int XMax = 16;
    int YMax = 16;
    int DifferenceX = TargetFramebuffer->Width - Position.X;
    int DifferenceY = TargetFramebuffer->Height - Position.Y;

    if (DifferenceX < 16) XMax = DifferenceX;
    if (DifferenceY < 16) YMax = DifferenceY;

    for (int Y = 0; Y < YMax; Y++){
        for (int X = 0; X < XMax; X++){
            int Bit = Y * 16 + X;
            int Byte = Bit / 8;
            if ((MouseCursor[Byte] & (0b10000000 >> (X % 8))))
            {
                if (GetPix(Position.X + X, Position.Y + Y) == MouseCursorBufferAfter[X + Y *16]){
                    PutPix(Position.X + X, Position.Y + Y, MouseCursorBuffer[X + Y * 16]);
                }
            }
        }
    }
}

void DebugConsole::DrawOverlayMouseCursor(uint8_t* MouseCursor, Point Position, uint32_t Colour){


    int XMax = 16;
    int YMax = 16;
    int DifferenceX = TargetFramebuffer->Width - Position.X;
    int DifferenceY = TargetFramebuffer->Height - Position.Y;

    if (DifferenceX < 16) XMax = DifferenceX;
    if (DifferenceY < 16) YMax = DifferenceY;

    for (int Y = 0; Y < YMax; Y++){
        for (int X = 0; X < XMax; X++){
            int Bit = Y * 16 + X;
            int Byte = Bit / 8;
            if ((MouseCursor[Byte] & (0b10000000 >> (X % 8))))
            {
                MouseCursorBuffer[X + Y * 16] = GetPix(Position.X + X, Position.Y + Y);
                PutPix(Position.X + X, Position.Y + Y, Colour);
                MouseCursorBufferAfter[X + Y * 16] = GetPix(Position.X + X, Position.Y + Y);

            }
        }
    }

    MouseDrawn = true;
}


void DebugConsole::ClearChar(){

    if (CursorPosition.X == 0){
        CursorPosition.X = TargetFramebuffer->Width;
        CursorPosition.Y -= 16;
        if (CursorPosition.Y < 0) CursorPosition.Y = 0;
    }

    unsigned int xOff = CursorPosition.X;
    unsigned int yOff = CursorPosition.Y;

    unsigned int* pixPtr = (unsigned int*)TargetFramebuffer->BaseAddress;
    for (unsigned long y = yOff; y < yOff + 16; y++){
        for (unsigned long x = xOff - 8; x < xOff; x++){
            *(unsigned int*)(pixPtr + x + (y * TargetFramebuffer->PixelsPerScanLine)) = ClearColor;
        }
    }

    CursorPosition.X -= 8;

    if (CursorPosition.X < 0){
        CursorPosition.X = TargetFramebuffer->Width;
        CursorPosition.Y -= 16;
        if (CursorPosition.Y < 0) CursorPosition.Y = 0;
    }

}
