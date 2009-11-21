#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <khaos/kernel.h>
#include <khaos/driver.h>
#include <khaos/signal.h>

#include <driver/console.h>

#include "../libc/libc.h"

void swrite(const char *message) {
	console.write(0, strlen(message), (void*) message);
}

void death(uint32_t source, uint32_t args[4]) {
	return;
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
	swrite("ZOMG 42!!!");
}

char buffer2[100];
int main() {
	int pid;
	uint32_t args[4] = {0, 0, 0, 0};

	khsig_init();
	khsig_register(0, segfault);
	khsig_register(2, segfault);
	khsig_register(3, irq_handler);
	khsig_register(5, imgfault);
	khsig_register(42, stuff);

	console.init(0);

/*	print_bootsplash();
	update_progress("init system started...");

	update_progress("scanning disks...");
	if (ata.init(0) == 0) {
		update_progress("found ata");
	}
	else {
		update_progress("no disk found on ata");
	} */

	pid = fork_call(0);
	if (pid < 0) khsig_send(-pid, 42, args);

	for(;;);
	return 0;
}
