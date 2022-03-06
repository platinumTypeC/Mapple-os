#include <mapple/types.h>
#include <mapple/Memory.h>
#include <mapple/Gui.h>
#include <mapple/config.h>
#include <mapple/cstr.h>
#include <mapple/Interrupts.h>
#include <mapple/UserInput.h>
#include <cstddef>

DebugConsole* GloballConsole;
Framebuffer_t* GlobalFrameBuffer;
IDTR* GlobalIDTR;

void SetIDTGate(void* Handler, uint8_t EntryOffset, uint8_t TypeAttributes, uint8_t Selector){
    IDTDescEntry* Interrupt = (IDTDescEntry*)(GlobalIDTR->Offset + EntryOffset * sizeof(IDTDescEntry));
    Interrupt->SetOffset((uint64_t)Handler);
    Interrupt->TypeAttributes = TypeAttributes;
    Interrupt->Selector = Selector;
}

void PrepareIDT(){
    GlobalIDTR->Limit = 0x0FFF;
    GlobalIDTR->Offset = (uint64_t)GlobalAllocator->RequestPage();

    SetIDTGate((void*)PageFault_Handler, 0xE, IDT_TA_InterruptGate, 0x08);
    
    asm ("lidt %0" : : "m" (*GlobalIDTR));
}

extern "C" uint64_t kernel_main(
    BootInfo_t* boot_info
){
    GlobalFrameBuffer = &boot_info->frameBuffer;
    clearScreen();

    DebugConsole console = DebugConsole(&boot_info->defaultFont);
    GloballConsole = &console;
    DebugPrint("Starting Initialization\n");

    PrepareMemory(boot_info);

    PrepareIDT();

    InitPS2Mouse();

    DebugPrint("Done.. Haulting\n");
    asm("hlt");

    return 0;
};