#define NO_LL
#include <mapple/types.h>
#include <mapple/Gui.h>
#include <mapple/Interrupts.h>
#include <mapple/IO.h>
#include <mapple/UserInput.h>
#include <mapple/scheduling.h>

__attribute__((interrupt)) void PageFault_Handler(struct interrupt_frame* frame){
    GloballConsole->Print("Page fault detected");
    while(true);
}

__attribute__((interrupt)) void DoubleFault_Handler(interrupt_frame* frame){
    GloballConsole->Print("Double Fault Detected");
    while(true);
}

__attribute__((interrupt)) void GPFault_Handler(interrupt_frame* frame){
    GloballConsole->Print("General Protection Fault Detected");
    while(true);
}

__attribute__((interrupt)) void KeyboardInt_Handler(interrupt_frame* frame){
    uint8_t scancode;
    asm volatile ("inb %1, %0"
    : "=a"(scancode)
    : "Nd"(0x60));

    HandleKeyboard(scancode);

    PIC_EndMaster();
}

__attribute__((interrupt)) void MouseInt_Handler(interrupt_frame* frame){

    uint8_t mouseData;
    asm volatile ("inb %1, %0"
    : "=a"(mouseData)
    : "Nd"(0x60));

    HandlePS2Mouse(mouseData);

    PIC_EndSlave();
}

__attribute__((interrupt)) void PITInt_Handler(interrupt_frame* frame){
    PIT::Tick();
    PIC_EndMaster();
}

void PIC_EndMaster(){
    OutByte(PIC1_COMMAND, PIC_EOI);
}

void PIC_EndSlave(){
    OutByte(PIC2_COMMAND, PIC_EOI);
    OutByte(PIC1_COMMAND, PIC_EOI);
}
   

void RemapPIC(){
    uint8_t a1, a2; 

    a1 = InByte(PIC1_DATA);
    IOWait();
    a2 = InByte(PIC2_DATA);
    IOWait();

    OutByte(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    IOWait();
    OutByte(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    IOWait();

    OutByte(PIC1_DATA, 0x20);
    IOWait();
    OutByte(PIC2_DATA, 0x28);
    IOWait();

    OutByte(PIC1_DATA, 4);
    IOWait();
    OutByte(PIC2_DATA, 2);
    IOWait();

    OutByte(PIC1_DATA, ICW4_8086);
    IOWait();
    OutByte(PIC2_DATA, ICW4_8086);
    IOWait();

    OutByte(PIC1_DATA, a1);
    IOWait();
    OutByte(PIC2_DATA, a2);
}
