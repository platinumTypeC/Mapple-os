#pragma once
#include <mapple/types.h>


class Proccess{
    uint64_t pid;
    uint64_t importance;
    uint8_t isWaiting;
    void wait();
};
