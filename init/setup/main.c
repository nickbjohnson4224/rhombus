#include <lib.h>

int *signal_table = (void*) 0xF7FFF000;

char keymap[128] = "\0\0331234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;\'`\0\\zxcvbnm,./\0*\0 ";
char upkmap[128] = "\0\033!@#$%^&*()_+\b\0QWERTYUIOP{}\n\0ASDFGHJKL:\"~\0|ZXCVBNM<>?\0*\0 ";
short int *vmem = (void*) 0x20000000;

int up;
void kb_handler() {
	char c = inb(0x60);
	if (c & 0x80) {
		if (keymap[c & 0x7F] == '\0') up = 0;
		sret(3);
	}
	c = (up) ? upkmap[(int) c] : keymap[(int) c];
	if (c == '\0') {
		up = 1;
		sret(3);
	}

//*/
/*/	char m[] = {c, 0};
/*/	printf("%c", c);
/*/	eout(m);
//*/

	sret(3);
}

int pid;
int init(u32int eflags) {
	fmap(0, (addr_t) vmem, 0xB8000, 4000, 0x7);
	curse(0, 17);
	printf("0x%x\n", eflags);

	rirq(1);
	signal_table[3] = (int) kb_handler;

	up = 0;

	for(;;);
	return 0;
}
