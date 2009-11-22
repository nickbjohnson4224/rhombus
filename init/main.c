#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <khaos/kernel.h>
#include <khaos/driver.h>
#include <khaos/signal.h>

#include <driver/console.h>

#include "../libc/libc.h"

static void swrite(const char *message) {
	console.write(0, strlen(message), (void*) message);
}

static void print_bootsplash() {
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

int main() {
	int pid;
	uint32_t args[4] = {0, 0, 0, 0};

	khsig_register(3, irq_handler);
	khsig_register(42, stuff);

	console.init(0);
	rirq(console.interrupt, (uint32_t) console.handler);
	print_bootsplash();

	pid = fork_call(0);
	if (pid < 0) khsig_send(-pid, 42, args);

	for(;;);
	return 0;
}
