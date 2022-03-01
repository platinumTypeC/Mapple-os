#pragma once
#include <stddef.h>
#include <stdint.h>

class Bitmap_t{
    public:
    size_t Size;
    uint8_t* Buffer;
    bool operator[](uint64_t index);
    bool Set(uint64_t index, bool value);
    bool Get(uint64_t index);
};

#define Bitmap Bitmap_t