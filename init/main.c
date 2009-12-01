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

/*static void print_bootsplash() {
swrite("\n\
\t\t\t\t\t\t\t           '^a,\n\
\t\t\t\t\t\t\t        ,.    'b.\n\
\t\t\t\t\t\t\t      .d'       b.\n\
\t\t\t\t\t\t\t      S:        a:\n\
\t\t\t\t\t\t\t      'q,       p'\n\
\t\t\t\t\t\t\t        \"'    .p'\n\
\t\t\t\t\t\t\t           .,a'\n\
\t\t\t\t\t _  _   _   _   _____   _____   _____ \n\
\t\t\t\t\t| |/ / | |_| | |___  | |  _  | |  ___|\n\
\t\t\t\t\t|   <  |  _  | |  _  | | |_| | |___  |\n\
\t\t\t\t\t|_|\\_\\ |_| |_| |_____| |_____| |_____|\n\
\t\t\t\t\t          -= Version 0.1a =-\n");
}*/

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

void segfault(uint32_t source, uint32_t args[4]) {
	swrite("\nSegmentation Fault");
	exit_call(1);
}

void xwrite(uint32_t addr) {
	char m[9];
	const char *d = "0123456789ABCDEF";
	size_t i;

	for (i = 7; (int) i >= 0; i--) {
		m[i] = d[addr & 0xF];
		addr >>= 4;
	}

	m[8] = '\0';
	swrite((const char*) m);
}

int main() {
	size_t i;
	uint32_t *p;

	khsig_register(0, segfault);
	khsig_register(2, segfault);
	khsig_register(3, irq_handler);
	khsig_register(42, stuff);

	console.init(0);
	rirq(console.interrupt, (uint32_t) console.handler);
/*	print_bootsplash(); */

	swrite("Allocator test:\n");
	for (i = 0; i < 8; i++) {
		xwrite((uint32_t) (p = malloc(2 * sizeof(void*))));
		*p = 0x12345678;
		free(p);
		swrite("\n");
	}

	for(;;);
	return 0;
}
