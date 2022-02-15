#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <mapple/bootInfo.h>
#include <mapple/shedular.h>

void kernel_main(Boot_Info* boot_info) {
	shedular(boot_info);
	while(1);
}