#include <lib.h>
#include <stdint.h>

/***** INIT *****/

void death() {
}

void segfault() {
	eout("segfault\n");
	exit(1);
}

int init() {
	eout("    Init:");

	rsig(0, (uint32_t) segfault);
	rsig(2, (uint32_t) segfault);

	printf("Hello, world!\n");

	for(;;);
	return 0;
}
