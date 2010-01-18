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
	swrite("Segmentation Fault\n");
	exit(1);
}

void tisoverflow(uint32_t source, void *grant) {
}

int main() {
	extern void console_init(void);
	extern void shell(void);

	sigregister(SSIG_FAULT, segfault);
	sigregister(SSIG_PAGE, 	segfault);
	sigregister(SSIG_IMAGE, tisoverflow);

	console_init();
	shell();

	block();
	for(;;);
	return 0;
}
