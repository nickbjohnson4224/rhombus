#include <stdint.h>
#include <khaos/kernel.h>
#include <khaos/driver.h>
#include <khaos/exec.h>
#include <console.h>
#include <floppy.h>

void death(int eax) {
	sret_call(3);
}

void segfault(int eax) {
	swrite("\nSegmentation Fault\n");
	exit_call(1);
}

void imgfault(int eax) {
	swrite("Image Stack Overflow (DoS)");
	exit_call(1);
}

void (*signal_table[256])(int);
void csig_handler(int sig, int eax) {
	signal_table[sig](eax);
}

void (*irq_table[16])(void);
void irq_handler(int irq) {
	irq_table[irq]();
}

void rsig(int sig, uint32_t handler) {
	signal_table[sig] = (void (*)(int)) handler;
}

void rirq(int irq, uint32_t handler) {
	irq_table[irq] = (void (*)(void)) handler;
	rirq_call(irq);
}

char buffer2[100];
int init() {
	extern void sig_handler(void);

	sreg_call((uint32_t) sig_handler);
	rsig(0, (uint32_t) segfault);
	rsig(2, (uint32_t) segfault);
	rsig(5, (uint32_t) imgfault);
	rsig(3, (uint32_t) irq_handler);
	rirq(1, (uint32_t) kbhandle);
	rirq(floppy.interrupt, (uint32_t) floppy.handler);

	if (gpid_call() != 1) for(;;);

	print_bootsplash();
	update_progress("init system started...");

	update_progress("scanning for floppy drive...");
	floppy.init(0);

	for(;;);
	return 0;
}
