#include <stdint.h>
#include <khaos/kernel.h>
#include <khaos/driver.h>
#include <khaos/exec.h>
#include <khaos/signal.h>
#include <console.h>
#include <floppy.h>

void death(uint32_t source, uint32_t args[4]) {
	sret_call(3);
}

void segfault(uint32_t source, uint32_t args[4]) {
	swrite("\nSegmentation Fault\n");
	exit_call(1);
}

void imgfault(uint32_t source, uint32_t args[4]) {
	swrite("Image Stack Overflow (DoS)");
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

	khsignal_init();

	khsignal_register(0, segfault);
	khsignal_register(2, segfault);
	khsignal_register(3, irq_handler);
	khsignal_register(5, imgfault);

	rirq(1, (uint32_t) kbhandle);
	rirq(floppy.interrupt, (uint32_t) floppy.handler);

	if (gpid_call() != 1) for(;;);

	print_bootsplash();
	update_progress("init system started...");

	update_progress("scanning for floppy drive...");
	floppy.init(0);

	for(;;);
	return 0;
}
