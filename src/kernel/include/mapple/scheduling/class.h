#pragma once
#include <mapple/scheduling/Proccess.h>

class Scheduler{
    public:
        Proccess* TopStack;
        Proccess* MiddleStack;
        Proccess* BottomStack;
        Proccess* WaitingStack;

        uint64_t time;
        const uint64_t timeBlock = 1000;

        void tick();
        void addProcess(Proccess process);
};
