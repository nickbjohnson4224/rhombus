#include <stdint.h>
#include <khaos/kernel.h>
#include <khaos/driver.h>
#include <khaos/signal.h>
#include <driver/console.h>
#include <driver/ata.h>
#include <stdlib.h>
#include "../libc/libc.h"

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

void stuff(uint32_t source, uint32_t args[4]) {
	swrite("aaaaa");
}

char buffer2[100];
int main() {
	int pid;
	uint32_t args[4] = {0, 0, 0, 0};

	khsignal_init();
	khsignal_register(0, segfault);
	khsignal_register(2, segfault);
	khsignal_register(3, irq_handler);
	khsignal_register(5, imgfault);
	khsignal_register(42, stuff);

	print_bootsplash();
	update_progress("init system started...");

	update_progress("scanning disks...");
	if (ata.init(0) == 0) {
		update_progress("found ata");
	}
	else {
		update_progress("no disk found on ata");
	}

	pid = fork_call(0);
	if (pid < 0) khsignal_send(-pid, 42, args);

	for(;;);
	return 0;
}
