#include <lib.h>
#include <call.h>
#include <event.h>

void setup() {
	rsig(0, (u32int) segfault);
	rsig(2, (u32int) segfault);

	init_load_init();
	for(;;);
}
