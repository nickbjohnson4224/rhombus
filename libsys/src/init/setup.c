#include <lib.h>
#include <call.h>
#include <event.h>

u32int *signal_table = (void*) 0xF3FFF000;

void setup() {
	eout("  Libsys: registering handlers");
	eout("\t\t\t\t\t\t\t\t\t\t\t  [done]");

	u32int i;
	for (i = 0; i < 1024; i++) signal_table[i] = (u32int) NULL;
	signal_table[0] = (u32int) segfault;
	signal_table[2] = (u32int) segfault;

	init_load_init();
}
