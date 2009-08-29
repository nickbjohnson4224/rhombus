#include <lib.h>
#include <stdint.h>
#include <kernel.h>
#include <driver.h>

void death() {
	sret_call(3);
}

void segfault() {
	eout_call("Page Fault\n");
	exit_call(1);
}

void gepfault() {
	eout_call("General Protection Fault\n");
	exit_call(1);
}

void imgfault() {
	eout_call("Image Stack Overflow (DoS)");
	exit_call(1);
}

char buffer2[100];
int init() {
	eout_call("    Init:");

	rsig_call(0, (uint32_t) gepfault);
	rsig_call(2, (uint32_t) segfault);
	rsig_call(5, (uint32_t) imgfault);

	char buffer1[100];

	strcpy(buffer2, (char*) "hot potato");

	while(1) {
		eout_call("2 -> 1: ");
		buffer1[0] = 0;
		push_call(1, (uint32_t) buffer1, (uint32_t) buffer2, strlen(buffer2));
		eout_call(buffer1);

		eout_call("2 <- 1: ");
		buffer2[0] = 0;
		pull_call(1, (uint32_t) buffer1, (uint32_t) buffer2, strlen(buffer1));
		eout_call(buffer2);

		eout_call("1 <- 2: ");
		buffer1[0] = 0;
		pull_call(1, (uint32_t) buffer2, (uint32_t) buffer1, strlen(buffer2));
		eout_call(buffer1);

		eout_call("1 -> 2: ");
		buffer2[0] = 0;
		push_call(1, (uint32_t) buffer2, (uint32_t) buffer1, strlen(buffer1));
		eout_call(buffer2);
	}

	for(;;);
	return 0;
}
