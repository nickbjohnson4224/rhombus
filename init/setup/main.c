#include <lib.h>

#define KHAOS_VERSION_MAJOR 0
#define KHAOS_VERSION_MINOR 1

char *stamp = "\
Khaos Operating System v%d.%d\n\
Copyright 2009 Nick Johnson\n\
===========================\n\n";

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

void death() {
	printf("!");
}

char *stuff = (void*) 0x200000;
int othertask() {
	strcpy(stuff, "Potato Gravy!\n");
	push(1, (u32int) stuff, (u32int) stuff, 1024);
	while (sint(1, 16, 0, 0, 0, 0, 1));
	for(;;);
}

int pid;
void print() {
	printf("PRINT: ");
	printf("%s", stuff);
	sret(3);
}

short *vmem = (void*) 0x100000;
int init() {
/*	mmap((u32int) stuff, 1024, 0x7);
	mmap((u32int) vmem, 4000, 0x7);
	cleark();
	curse(0, 0);

	printf(stamp, KHAOS_VERSION_MAJOR, KHAOS_VERSION_MINOR);

	rsig(3, (u32int) kb_handler);*/
	rsig(7, (u32int) death);
//	rsig(16, (u32int) print);
//	up = 0;
//	rirq(1);

//	if ((pid = fork()) < 0) othertask();
	while(1) {
		fork();
//		if (fork() < 0) exit(0);
	}

	for(;;);// printf("A");
	return 0;
}
