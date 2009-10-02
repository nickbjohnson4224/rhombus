#include <stdint.h>
#include <kernel.h>
#include <driver.h>
#include <exec.h>
#include "console.h"

void death() {
	sret_call(3);
}

void segfault() {
	uint32_t addr, err, ip;
/*	char buffer[10]; */

	asm volatile ("mov %%edx, %0" : "=r" (addr));
	asm volatile ("mov %%ebx, %0" : "=r" (err));
	asm volatile ("mov %%eax, %0" : "=r" (ip));

	swrite("\n");
	swrite("Page Fault");
/*	swrite(itoa(addr, buffer, 16));
	swrite(" flags ");
	swrite(itoa(err, buffer, 16));
	swrite(" instruction ");
	swrite(itoa(ip, buffer, 16)); */
	swrite("\n");
	exit_call(1);
}

void gepfault() {
	swrite("General Protection Fault\n");
	exit_call(1);
}

void imgfault() {
	swrite("Image Stack Overflow (DoS)");
	exit_call(1);
}


uint8_t keymap[128] = "\0\0331234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;\'`\0\\zxcvbnm,./\0*\0 ";
uint8_t upkmap[128] = "\0\033!@#$%^&*()_+\b\0QWERTYUIOP{}\n\0ASDFGHJKL:\"~\0|ZXCVBNM<>?\0*\0 ";
uint8_t shift = 0;
void kbhandle() {
	char c = inb(0x60);
	if (c & 0x80) {
		if (keymap[(int) c & 0x7F] == '\0') shift = 0;
		sret_call(3);
	}
	if (keymap[(int) c & 0x7F] == '\0') shift = 1;
	c = (shift) ? upkmap[(int) c] : keymap[(int) c];
	cwrite(c);
	sret_call(3);
}

void (*signal_table[256])(void);
void csig_handler(int sig) {
	signal_table[sig]();
}
void rsig(int sig, uint32_t handler) {
	signal_table[sig] = (void (*)(void)) handler;
}

char buffer2[100];
int init() {
	extern void sig_handler(void);

	sreg_call((uint32_t) sig_handler);
	rsig(0, (uint32_t) gepfault);
	rsig(2, (uint32_t) segfault);
	rsig(5, (uint32_t) imgfault);
	rsig(3, (uint32_t) kbhandle);
	rirq_call(1);

	if (gpid_call() != 1) for(;;);

	print_bootsplash();
	update_progress("init system started");

	for(;;);
	return 0;
}
