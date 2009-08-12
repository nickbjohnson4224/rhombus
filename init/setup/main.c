#include <lib.h>

int *signal_table = (void*) 0xF7FFF000;

char keymap[128] = "\0\0331234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;\'`\001\\zxcvbnm,./\001*\0 ";
char upkmap[128] = "\0\033!@#$%^&*()_+\b\0QWERTYUIOP{}\n\0ASDFGHJKL:\"~\001|ZXCVBNM<>?\001*\0 ";

int up;
void kb_handler() {
	char c = inb(0x60);
	if (keymap[c & 0x7F] == '\001') up ^= 1;
	if (c & 0x80 || keymap[(int) c] == '\001') sret(0);
	c = (up) ? upkmap[(int) c] : keymap[(int) c];
	if (c) printf("%c", c);
	sret(0);
}

void death() {
	printf("!");
}

short *vmem = (void*) 0x100000;
int init() {
	mmap((u32int) vmem, 4000, 0x7);
	cleark();
	curse(0, 0);

	signal_table[3] = (int) kb_handler;
	signal_table[7] = (int) death;
	rirq(1);

	for(;;);
	return 0;
}
