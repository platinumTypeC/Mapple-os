#pragma once
#include <mapple/scheduling/Proccess.h>

namespace Scheduler {
    void addProcess(void* start);
    void removeProcess(uint8_t pos);
    void removeProcess(uint16_t pid);
    void removeProcess(Proccess* process);
    void removeProcess(Proccess process);

    // TODO: Note all of them have been implemented yet
}
