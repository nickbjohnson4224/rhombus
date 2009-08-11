#include <lib.h>

char keymap[128] = "\0\0331234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;\'`\001\\zxcvbnm,./\001*\0 ";
char upkmap[128] = "\0\033!@#$%^&*()_+\b\0QWERTYUIOP{}\n\0ASDFGHJKL:\"~\001|ZXCVBNM<>?\001*\0 ";
char line_buffer[1024];
u32int line_buffer_top = 0;
int line_done = 0;

int up;
void kb_handler() {
	char c = inb(0x60);
	if (keymap[c & 0x7F] == '\001') up ^= 1;
	if (c & 0x80 || keymap[(int) c] == '\001') sret(0);
	c = (up) ? upkmap[(int) c] : keymap[(int) c];
	printf("%c", c);
	if (c == '\n') line_done = 1;
	else if (c == '\b' && line_buffer_top > 0) line_buffer_top--;
	else line_buffer[line_buffer_top++] = c;
	sret(0);
}

void read_handler(u32int target, addr_t base) {
//	printf("READ: %d:%x\n", target, base);
	fmap(target, (addr_t) wrtbuffer, base, 1024, 0x7);
	volatile char *rbase = (void*) ((base % PAGESZ) + (u32int) wrtbuffer);
	
	while (!line_done);

	u32int i;
	for (i = 0; i < line_buffer_top; i++)
		rbase[i] = line_buffer[i];
	line_buffer_top = 0;
	rbase[i] = '\0';
	line_done = 0;

	sret(3);
}

short int *vmem = (void*) 0x20000000;
extern void read_handler_stub();
extern void write_handler_stub();
void init_cdrv() {

	fmap(0, (addr_t) vmem, 0xB8000, 4000, 0x7);
	curse(0, 17); 
	printf("    Init: launching console driver");

		up = 0;

		signal_table[3] = (int) kb_handler;
		signal_table[16] = (int) read_handler_stub;
		signal_table[17] = (int) write_handler_stub;
		rirq(1);

	curse(74, -1); 
	printf("[done]");

	for(;;);
}

