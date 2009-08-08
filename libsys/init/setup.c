#include <lib.h>
#include <call.h>
#include <event.h>

u32int *signal_table = (void*) 0xF7FFF000;

__attribute__ ((section(".stbl")))
u32int sigovr_table[] = {
(u32int) segfault, 
(u32int) NULL, 
(u32int) segfault
};

void setup() {
	eout("  Libsys: clearing signal table");
	eout("\t\t\t\t\t\t\t\t\t\t\t  [done]");

	u32int i;
	for (i = 0; i < 1024; i++) signal_table[i] = (u32int) NULL;

	init_load_init();
	for(;;);
}
