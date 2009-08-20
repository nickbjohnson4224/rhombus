#include <lib.h>
#include <stdint.h>
#include <driver.h>

void death() {
	sret(3);
}

void segfault() {
	uint32_t addr;
	asm volatile ("mov %%edx, %0" : "=r" (addr));
	printk("segfault @ 0x%x\n", addr);
	exit(1);
}

char buffer2[100];
int init() {
	eout("    Init:");

	rsig(0, (uint32_t) segfault);
	rsig(2, (uint32_t) segfault);

	mmap(0x100000, 4000, 0x7);
	printf("Hello, world!\n");

	char buffer1[100];

	strcpy(buffer2, "hot potato");

	printf("\n2 -> 1\n");
	buffer1[0] = 0;
	push_call(1, (uint32_t) buffer1, (uint32_t) buffer2, strlen(buffer2));
	printf("1: %s\n", buffer1);

	printf("\n2 <- 1\n");
	buffer2[0] = 0;
	pull_call(1, (uint32_t) buffer1, (uint32_t) buffer2, strlen(buffer1));
	printf("2: %s\n", buffer2);

	printf("\n1 <- 2\n");
	buffer1[0] = 0;
	pull_call(1, (uint32_t) buffer2, (uint32_t) buffer1, strlen(buffer2));
	printf("1: %s\n", buffer1);

	printf("\n1 -> 2\n");
	buffer2[0] = 0;
	push_call(1, (uint32_t) buffer2, (uint32_t) buffer1, strlen(buffer1));
	printf("2: %s\n", buffer1);

	printf("\ndone.\n");

	for(;;);
	return 0;
}
