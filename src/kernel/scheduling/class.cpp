#include <mapple/scheduling.h>
#include <mapple/Memory.h>
#include <mapple/Gui.h>

namespace Scheduler {
    namespace 
    {
        LIST_INIT(TopStack);
        LIST_INIT(MiddleStack);
        LIST_INIT(BottomStack);
        LIST_INIT(WaitingStack);
    }

    void addProcess(uint8_t pid)
    {
        Proccess process;
        process.pid = pid;

        list_add(&process.node, &TopStack);
    };

    void removeProcess(uint8_t pid)
    {
    };
}
