#define NO_LL
#include <mapple/types.h>
#include <mapple/Gui.h>
#include <mapple/Interrupts.h>

__attribute__((interrupt)) void PageFault_Handler(struct interrupt_frame* frame){
    GloballConsole->Print("Page fault detected");
    asm("hlt");
}
