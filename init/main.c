#include <stdint.h>
#include <khaos/kernel.h>
#include <khaos/driver.h>
#include <khaos/exec.h>
#include <khaos/signal.h>
#include <driver/console.h>
#include <driver/ata.h>
#include <stdlib.h>

void death(uint32_t source, uint32_t args[4]) {
	sret_call(3);
}

void segfault(uint32_t source, uint32_t args[4]) {
	swrite("\nSegmentation Fault\n");
	exit_call(1);
}

void imgfault(uint32_t source, uint32_t args[4]) {
	swrite("\nImage Stack Overflow (DoS)\n");
	exit_call(1);
}

void (*irq_table[16])(void);
void irq_handler(uint32_t source, uint32_t args[4]) {
	if (irq_table[args[0] & 0xF]) irq_table[args[0] & 0xF]();
}

void rirq(int irq, uint32_t handler) {
	irq_table[irq] = (void (*)(void)) handler;
	rirq_call(irq);
}

char buffer2[100];
int init() {
	void *m;
	double d;

	khsignal_init();
	khsignal_register(0, segfault);
	khsignal_register(2, segfault);
	khsignal_register(3, irq_handler);
	khsignal_register(5, imgfault);

	print_bootsplash();
	update_progress("init system started...");

	update_progress("scanning disks...");
	if (ata.init(0) == 0) {
		update_progress("found ata");
	}
	else {
		update_progress("no disk found on ata");
	}

	nwrite(0, 16);

	init_heap();
	swrite("\n");

	nwrite((uint32_t) (m = calloc(256, sizeof(int))), 16);
	swrite("\n");
	nwrite((uint32_t) calloc(256, sizeof(int)), 16);
	free(m);
	swrite("\n");
	nwrite((uint32_t) calloc(256, sizeof(int)), 16);

	swrite("\n");
	d = 42.0 / 5.0;
	d *= 5.0;
	nwrite((int) d, 10);

	curse(24, 79);
	swrite("@");

	curse(0,0);
	swrite("@");

	for(;;);
	return 0;
}
