#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <mapple/mapple.h>

void kernel_main(Boot_Info* boot_info) {
	init_graphics(boot_info);
	init_pci_bus(boot_info);
	
	while(1);
}