#include <mapple/types.h>
#include <mapple/Memory.h>
#include <mapple/Gui.h>
#include <mapple/config.h>
#include <mapple/cstr.h>
#include <mapple/Interrupts.h>
#include <cstddef>

DebugConsole* GloballConsole;
Framebuffer_t* GlobalFrameBuffer;
IDTR* GlobalIDTR;

extern "C" uint64_t kernel_main(
    BootInfo_t* boot_info
){
    GlobalFrameBuffer = &boot_info->frameBuffer;
    clearScreen();

    DebugConsole console = DebugConsole(&boot_info->defaultFont);
    GloballConsole = &console;
    DebugPrint("Starting Initialization\n");

    PrepareMemory(boot_info);
    
    GlobalIDTR->Limit = 0x0FFF;
    GlobalIDTR->Offset = (uint64_t)GlobalAllocator->RequestPage();

    IDTDescEntry* int_PageFault = (IDTDescEntry*)(GlobalIDTR->Offset + 0xE * sizeof(IDTDescEntry));
    int_PageFault->SetOffset((uint64_t)PageFault_Handler);
    int_PageFault->TypeAttributes = IDT_TA_InterruptGate;
    int_PageFault->Selector = 0x08;

    DebugPrint("Done.. Haulting\n");
    asm("hlt");

    return 0;
};