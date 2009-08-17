#include <lib.h>

#define KHAOS_VERSION_MAJOR 0
#define KHAOS_VERSION_MINOR 1

char *stamp = "\
Khaos Operating System v%d.%d\n\
Copyright 2009 Nick Johnson\n\
===========================\n\n";

/***** COMMON *****/

char *buffer = (void*) 0x200000;

static void sleep(int cycles) {
	volatile int i;
	for (i = 0; i < cycles; i++);
}

/***** CONSOLE DRIVER *****/

char keymap[128] = "\0\0331234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;\'`\001\\zxcvbnm,./\001*\0 ";
char upkmap[128] = "\0\033!@#$%^&*()_+\b\0QWERTYUIOP{}\n\0ASDFGHJKL:\"~\001|ZXCVBNM<>?\001*\0 ";

int up;
void kb_handler() {
	char c = inb(0x60);
	if (keymap[c & 0x7F] == '\001') up = (up) ? 0 : 1;
	if (c & 0x80 || keymap[(int) c] == '\001') sret(0);
	c = (up) ? upkmap[(int) c] : keymap[(int) c];
	if (c) printf("%c", c);
	sret(0);
}

short *vmem = (void*) 0x100000;

extern void print_stub();
void print_handler(int task, u32int addr, u32int size) {
	u32int i;

	printk("PRINT: %d:%x <%d\n", task, addr, size);

	pull(task, addr, (u32int) buffer, size);
	for (i = 0; i < size; i++) {
		printk("%c", buffer[i]);
	}

	sret(3);
}

int cdrv_init() {
	mmap((u32int) buffer, 0x1000, 0x7);

	mmap((u32int) vmem, 4000, 0x7);
	cleark();
	curse(0, 0);

	rsig(3, (u32int) kb_handler);
	rsig(16, (u32int) print_stub);
	rirq(1);
	up = 0;

	printf("Hello, world!\n");

	for(;;);
}

/***** INIT *****/

int cdrv_pid;

void death() {
}

void print(char *message) {
	sint(cdrv_pid, 16, 0, 0, strlen(message), (u32int) message, 1);
}

int init() {
	eout("    Init:");

	mmap((u32int) buffer, 0x1000, 0x7);

//	rsig(7, (u32int) death);

//	if ((cdrv_pid = fork()) < 0) cdrv_init();

	sleep(10000);

	print("Wello, Horld!\n");

	for(;;);
	return 0;
}
