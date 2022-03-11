#include <error.h>
#include <mapple/types.h>
#include <mapple/Memory.h>
#include <mapple/Gui.h>
#include <mapple/config.h>
#include <mapple/cstr.h>
#include <mapple/Interrupts.h>
#include <mapple/UserInput.h>
#include <Drivers/ACPI.h>
#include <Drivers/PCI.h>
#include <EFIRT/time.h>
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

void PrepareACPI(BootInfo_t* boot_info){
    ACPI::RSDP2* rsdp = (ACPI::RSDP2*)boot_info->rsdp;

    ACPI::SDTHeader* xstd = (ACPI::SDTHeader*)rsdp->XSDTAddress;

    DebugPrint("Finding MCFG from ACPI Table\n");

    ACPI::MCFGHeader* mcfg = (ACPI::MCFGHeader*)ACPI::FindTable(xstd, (char*)"MCFG");
    
    PCI::EnumeratePCI(mcfg);
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

    DebugPrint("Preparing ACPI\n");
    
    PrepareACPI(boot_info);

    DebugPrint("Done Preparing ACPI\n");

    DebugPrint("Time: ");
    DebugPrint(to_string((uint64_t)getTime(boot_info).Hour));
    DebugPrint("\n");
    
    DebugPrint("Done.. Haulting\n");
    asm("hlt");

    return 0;
};
