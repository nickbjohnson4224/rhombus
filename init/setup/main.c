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

void kbhandle() {
	eout_call("kb");
	sret_call(3);
}

char buffer2[100];
int init() {

	rsig_call(0, (uint32_t) gepfault);
	rsig_call(2, (uint32_t) segfault);
	rsig_call(5, (uint32_t) imgfault);
	rsig_call(3, (uint32_t) kbhandle);
	rirq_call(1);

	eout_call("potato");

	for(;;);
	return 0;
}
