/* Copyright 2009 Nick Johnson */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <mmap.h>
#include <flux.h>

#include <driver.h>
#include <config.h>

void swrite(const char *message) {
	extern size_t console_write(char*, size_t);
	console_write((char*) message, strlen(message));
}

void segfault(uint32_t source, void *grant) {
	exit(1);
}

int main() {
	extern void console_init(void);

	sigregister(SSIG_FAULT, segfault);
	sigregister(SSIG_PAGE, segfault);

	console_init();
	swrite("hello, world!\n");
	swrite("potato cannon!\n");

	block();
	for(;;);
	return 0;
}
