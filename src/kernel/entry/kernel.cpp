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

    DebugPrint("Initializing Malloc \n");

    init_malloc();

    DebugPrint("Initialized Malloc \n");

    DebugPrint("Starting Malloc test\n");

    uint8_t n = 5;

    uint64_t* ptr = (uint64_t*)malloc(n * sizeof(uint64_t));

    if (ptr == NULL){
        DebugPrint("Memory alloc failed!\n");
    } else {
        DebugPrint("Memory Allocated Succesfully !!!\n");

        for (uint8_t i = 0; i < n; ++i)
        {
            ptr[i] = i + 1;
        };

        DebugPrint("Ptr: ");
        DebugPrintNum(ptr[0]);
        DebugPrint("\n");
    }

    free(ptr);

    DebugPrint("Pointer Freed\n");
    DebugPrint("Finished Malloc test\n");

    DebugPrint("Done.. Haulting\n");
    asm("hlt");

    return 0;
};
