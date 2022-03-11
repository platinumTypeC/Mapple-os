#include "../../../../include/efi.h"
#include "../../../../include/efilib.h"
#include <mapple/types.h>

EFI_TIME getTime(BootInfo_t* boot_info){
    EFI_TIME Time;
    boot_info->RT->GetTime (&Time, (EFI_TIME_CAPABILITIES*)NULL);
    return Time;
};
