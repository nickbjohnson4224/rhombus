#include <lib.h>
#include <stdint.h>
#include <driver.h>

void death() {
	sret(3);
}

void segfault() {
	eout("Page Fault\n");
	exit(1);
}

void gepfault() {
	eout("General Protection Fault\n");
	exit(1);
}

void imgfault() {
	eout("Image Stack Overflow (DoS)");
	exit(1);
}

char buffer2[100];
int init() {
	eout("    Init:");

	rsig(0, (uint32_t) gepfault);
	rsig(2, (uint32_t) segfault);
	rsig(5, (uint32_t) imgfault);

	char buffer1[100];

	strcpy(buffer2, (char*) "hot potato");

	while(1) {
		eout("2 -> 1: ");
		buffer1[0] = 0;
		push_call(1, (uint32_t) buffer1, (uint32_t) buffer2, strlen(buffer2));
		eout(buffer1);

		eout("2 <- 1: ");
		buffer2[0] = 0;
		pull_call(1, (uint32_t) buffer1, (uint32_t) buffer2, strlen(buffer1));
		eout(buffer2);

		eout("1 <- 2: ");
		buffer1[0] = 0;
		pull_call(1, (uint32_t) buffer2, (uint32_t) buffer1, strlen(buffer2));
		eout(buffer1);

		eout("1 -> 2: ");
		buffer2[0] = 0;
		push_call(1, (uint32_t) buffer2, (uint32_t) buffer1, strlen(buffer1));
		eout(buffer2);
	}

	for(;;);
	return 0;
}
