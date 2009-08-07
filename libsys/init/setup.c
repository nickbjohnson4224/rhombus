#include <lib.h>
#include <call.h>
#include <event.h>

extern int fork();

u32int *signal_table = (void*) 0xF7FFF000;

char keymap[128] = "\0\0331234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;\'`\0\\zxcvbnm,./\0*\0 ";
char upkmap[128] = "\0\033!@#$%^&*()_+\b\0QWERTYUIOP{}\n\0ASDFGHJKL:\"~\0|ZXCVBNM<>?\0*\0 ";

int *up = (void*) 0x200000;
int *top = (void*) 0x200004;
short int *vmem = (void*) 0x300000;

void kb_handler() {
	char c = inb(0x60);
	if (c & 0x80) {
		if (keymap[(int) c & 0x7F] == 0) *up = 0;
		sret(3);
	}
	if (keymap[(int) c] == 0) {
		*up = 1;
		sret(3);
	}
	c = (*up) ? upkmap[(int) c] : keymap[(int) c];
	vmem[(*top)++] = c | 0x0F00;

	if (*top >= 25*80) *top = 0;

	sret(3);
}

void death() {
	eout("child died ;-(\n");
	sret(3);
}

void setup() {
	eout("  Libsys: registering handlers");
	eout("\t\t\t\t\t\t\t\t\t\t\t  [done]");

	u32int i;
	for (i = 0; i < 1024; i++) signal_table[i] = (u32int) NULL;
	signal_table[0] = (u32int) segfault;
	signal_table[2] = (u32int) segfault;
	signal_table[3] = (u32int) kb_handler;
	signal_table[7] = (u32int) death;

	fmap(0, vmem, 0xB8000, 0x1000, 0x7);
	mmap(up, sizeof(int) * 2, 0x7);
	*up = *top = 0;
	int cpid;

	rirq(1);

	init_load_init();
	for(;;);
}
