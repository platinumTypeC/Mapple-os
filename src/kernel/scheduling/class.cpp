#include <mapple/scheduling.h>
#include <mapple/Memory.h>
#include <mapple/Gui.h>

namespace Scheduler {
    LIST_INIT(Stack);

    uint16_t pidCounter = 0;

    void addProcess(void* start)
    {
        Proccess process;
        process.pid = pidCounter++;
        process.importance = 1; // 0 is waiting, 1 is top, 2 is middle, 3 is bottom
        process.isWaiting = 0;
        process.start = start;

        list_add(&process.node, &Stack);
    };

    void removeProcess(uint16_t pid)
    {
        Proccess* proc = NULL;
        list_for_each_entry(proc, &Stack, node){
            DebugPrint("List\n");
        };
    };
}
