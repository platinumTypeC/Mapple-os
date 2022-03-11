#include <mapple/scheduling.h>
#include <mapple/IO.h>
#include <stdint.h>

namespace Scheduling {
    void Sleep(uint64_t milliseconds){
        uint64_t done;
        while (done <= milliseconds){
            done++;
        }
    }
}
