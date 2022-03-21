#pragma once
#include <mapple/types.h>


class Proccess{
    public:
    ll_t node;
    uint8_t pid;
    uint64_t importance;
    uint8_t isWaiting;
    void wait();
};
