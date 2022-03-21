#pragma once
#include <mapple/types.h>


class Proccess{
    public:
    ll_t node;
    uint16_t pid;
    uint8_t importance;
    uint8_t isWaiting;
    void* start;
};
