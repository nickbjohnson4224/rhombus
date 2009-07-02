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
	c = (up) ? upkmap[c] : keymap[c];
	if (c == '\0') up = 1;
	printk("%c", c);

	sret(3);
}

int pid;
int main() {
	eout("Hello, World\n");

	unsigned int src, dest;
	for (src = 0xB8000, dest = 0x20000000; src < 0xC0000; src += 0x1000, dest += 0x1000)
		fmap(0, src, dest);

	cleark();

	rirq(1);
	signal_table[3] = (int) kb_handler;

	up = 0;

	for(;;);
	return 0;
}
