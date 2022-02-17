#include <mapple/pciCheckBus.h>
void checkDevice(
    uint64_t bus,
    uint64_t device
){

};

void init_pci_bus(Boot_Info* boot_info){
	debug_patern(
		0x00de105b,
		0x00de105b
	);
    uint16_t bus;
    uint8_t device;

    for (bus = 0; bus < 256; bus++) {
        for (device = 0; device < 32; device++) {
            checkDevice(bus, device);
        }
    }
};