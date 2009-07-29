#include <lib.h>
#include <call.h>
#include <event.h>

u32int *signal_table = (void*) 0xF3FFF000;

char keymap[128] = "\0\0331234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;\'`\0\\zxcvbnm,./\0*\0 ";
char upkmap[128] = "\0\033!@#$%^&*()_+\b\0QWERTYUIOP{}\n\0ASDFGHJKL:\"~\0|ZXCVBNM<>?\0*\0 ";

int *up = 0x200000;

void kb_handler() {
	char c = inb(0x60);
	if (c & 0x80) sret(3);
	c = keymap[(int) c];
	char m[2] = {c, '\0'};
	eout(m);

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

	mmap(up, sizeof(int), 0x7);
	*up = 0x42242442;

	rirq(1);

//	init_load_init();
	for(;;);
}
