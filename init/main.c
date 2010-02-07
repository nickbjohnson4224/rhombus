/* Copyright 2009, 2010 Nick Johnson */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <flux.h>

#include <driver.h>
#include <config.h>

void segfault(uint32_t source, struct request *req) {
	if (req) req_free(req);

	printf("Segmentation Fault\n");
	exit(1);
}

int main() {
	extern void console_init(void);
	extern void disk_init(void);
	extern void shell(void);

	sigregister(SSIG_FAULT, segfault);
	sigregister(SSIG_PAGE, 	segfault);

	console_init();

	printf("Flux 0.2a booting...\n");

	disk_init();
	shell();

	for(;;);
	return 0;
}
