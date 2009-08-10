#include <lib.h>

int *signal_table = (void*) 0xF7FFF000;
#define PAGESZ 0x1000

/***** CONSOLE DRIVER *****/
char keymap[128] = "\0\0331234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;\'`\001\\zxcvbnm,./\001*\0 ";
char upkmap[128] = "\0\033!@#$%^&*()_+\b\0QWERTYUIOP{}\n\0ASDFGHJKL:\"~\001|ZXCVBNM<>?\001*\0 ";
short int *vmem = (void*) 0x20000000;
char *wrtbuffer = (void*) 0x30000000;

int up;
void kb_handler() {
	char c = inb(0x60);
	if (keymap[c & 0x7F] == '\001') up ^= 1;
	if (c & 0x80 || keymap[(int) c] == '\001') sret(3);
	printf("%c", (up) ? upkmap[(int) c] : keymap[(int) c]);
	sret(3);
}

extern void write_handler_stub();
void write_handler(u32int target, addr_t base, u32int size) {
	fmap(target, (addr_t) wrtbuffer, base, size, 0x7);
	char *wbase = (void*) ((base % PAGESZ) + (u32int) wrtbuffer);

	u32int i;
	for (i = 0; i < size; i++)
		printf("%c", wbase[i]);
}

void init_cdrv() {

	fmap(0, (addr_t) vmem, 0xB8000, 4000, 0x7);
	curse(0, 17); 
	printf("    Init: launching console driver");

		up = 0;

		signal_table[3] = (int) kb_handler;
		signal_table[17] = (int) write_handler_stub;
		rirq(1);

	curse(74, -1); 
	printf("[done]");

	for(;;);
}

/***** INIT *****/
#define CDRV 0
#define DDRV 1
int driverlist[10];

void sleep(int cycles) {
	volatile int i;
	for (i = 0; i < cycles; i++);
}

void print(char *message) {
	sint(driverlist[CDRV], 17, 0, 0, strlen(message), (u32int) message, 1);
}

int init() {
	int pid;

	if ((pid = fork()) < 0) init_cdrv();
	driverlist[CDRV] = pid;

	sleep(100000);
	print("Hello, World!\n");

	for(;;);
	return 0;
}
