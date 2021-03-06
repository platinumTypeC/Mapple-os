#include <mapple/types.h>
#include <mapple/Memory.h>
#include <mapple/Gui.h>
#include <mapple/config.h>
#include <mapple/cstr.h>
#include <mapple/Interrupts.h>
#include <mapple/UserInput.h>
#include <Drivers/ACPI.h>
#include <Drivers/PCI.h>
#include <mapple/scheduling.h>
#include <mapple/IO.h>

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
    SetIDTGate((void*)DoubleFault_Handler, 0x8, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)GPFault_Handler, 0xD, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)KeyboardInt_Handler, 0x21, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)MouseInt_Handler, 0x2C, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)PITInt_Handler, 0x20, IDT_TA_InterruptGate, 0x08);   

    asm ("lidt %0" : : "m" (*GlobalIDTR));

    RemapPIC();
}

void PrepareACPI(BootInfo_t* boot_info){
    ACPI::RSDP2* rsdp = (ACPI::RSDP2*)boot_info->rsdp;

    ACPI::SDTHeader* xstd = (ACPI::SDTHeader*)rsdp->XSDTAddress;

    DebugPrint("Finding MCFG from ACPI Table\n");

    ACPI::MCFGHeader* mcfg = (ACPI::MCFGHeader*)ACPI::FindTable(xstd, (char*)"MCFG");
    
    PCI::EnumeratePCI(mcfg);
}

void MallocTest()
{
    for (uint8_t i = 0; i < 10; i++){
        void* ptr = malloc(sizeof(int) * 16);
        if (ptr != NULL)
        {
            free(ptr);
        } else 
        {
            DebugPrint("Failed Test \n");
            while (true);
            return;
        }
    }
};

extern "C" uint64_t kernel_main(
    BootInfo_t* boot_info
){
    GlobalFrameBuffer = &boot_info->frameBuffer;
    clearScreen();

    DebugConsole console = DebugConsole(&boot_info->defaultFont);
    GloballConsole = &console;

    PrepareMemory(boot_info);

    PrepareIDT();

    InitPS2Mouse();
    
    PrepareACPI(boot_info);

    OutByte(PIC1_DATA, 0b11111000);
    OutByte(PIC2_DATA, 0b11101111);

    asm("sti");

    DebugPrint("Done Prepared ACPI\n");

    init_malloc();

    DebugPrint("Initialized Malloc \n");

    MallocTest();

    DebugPrint("Successfully Passed Malloc test\n");

    DebugPrint("Time Since Boot: ");
    DebugPrintNum(PIT::TimeSinceBoot);
    DebugPrint("\n");

    Scheduler::addProcess((void*)0x100000); // TODO: Load a test binary here

    DebugPrint("Started Scheduling for multTasking\n");
    DebugPrint("Done.. Haulting\n");

    while(true);

    return 0;
};
