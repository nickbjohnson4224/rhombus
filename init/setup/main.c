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

/***** VGA DRIVER *****/

#define VGA_WIDTH 256
#define VGA_HEIGHT 256

extern void draw_stub();
extern void flip_stub();

u8int *vga_buffer = (void*) 0x300000;
int vdrv_init() {
	mmap((u32int) vga_buffer, 65536, 0x7);
	memset(vga_buffer, 0, 65536);

	vga_init(VGA_WIDTH, VGA_HEIGHT, 1);
	vga_flip((u32int) vga_buffer);

	rsig(16, (u32int) draw_stub);
	rsig(18, (u32int) flip_stub);

	for(;;);
}

void draw_handler(u16int x, u16int y, u8int color) {
	vga_plot(x, y, color, vga_buffer);
	sret(3);
}

void flip_handler() {
	vga_flip((u32int) vga_buffer);
	sret(3);
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

	if (!pull(task, addr, (u32int) buffer, size)) {
		printk("CDRV Error\n");
		sret(3);
	}
	for (i = 0; i < size; i++) {
		printk("%c", buffer[i]);
	}

	sret(3);
}

int cdrv_init() {
	mmap((u32int) buffer, 0x1000, 0x7);

	mmap((u32int) vmem, 4000, 0x7);
	curse(0, 0);

	rsig(3, (u32int) kb_handler);
	rsig(16, (u32int) print_stub);
	rirq(1);
	up = 0;

	printf("Hello, world!\n");

	for(;;);
}

/***** INIT *****/

int cdrv_pid, vdrv_pid;

void death() {
}

void print(char *message) {
	sint(cdrv_pid, 16, 0, 0, strlen(message), (u32int) message, 1);
}

void draw(int x, int y, char color) {
	sint(vdrv_pid, 16, (u32int) x, (u32int) y, 0, (u32int) color, 1);
}

void flip() {
	sint(vdrv_pid, 18, 0, 0, 0, 0, 0);
}

int init() {
	eout("    Init:");

	rsig(7, (u32int) death);

//	if ((cdrv_pid = fork()) < 0) cdrv_init();
	if ((vdrv_pid = fork()) < 0) vdrv_init();

	sleep(100);

	u32int i;
	for (i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
		draw(i % VGA_WIDTH, i / VGA_HEIGHT, (~((i % VGA_WIDTH) ^ (i / VGA_HEIGHT)) & 0x3F) + 64);

	flip();

	outb(0x64, 0xFE);

	for(;;);
	return 0;
}
