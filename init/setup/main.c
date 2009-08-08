#include <lib.h>

extern void eout(char *message);

int *signal_table = (void*) 0xF3FFF000;

char keymap[128] = "\0\0331234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;\'`\0\\zxcvbnm,./\0*\0 ";
char upkmap[128] = "\0\033!@#$%^&*()_+\b\0QWERTYUIOP{}\n\0ASDFGHJKL:\"~\0|ZXCVBNM<>?\0*\0 ";
short int *vmem = (void*) 0x20000000;

int up;
void kb_handler() {
	char c = inb(0x60);
	if (c & 0x80) {
		up = 0;
		sret(3);
	}
	c = (up) ? upkmap[(int) c] : keymap[(int) c];
	if (c == '\0') up = 1;
	printk("%c", c);

	sret(3);
}

int pid;
int init() {
	eout("Wello, Horld!\n");

	for(;;);

	cleark();

	rirq(1);
	signal_table[3] = (int) kb_handler;

	up = 0;

	for(;;);
	return 0;
}
