// Copyright 2009 Nick Johnson

#include <lib.h>
#include <driver.h>

/*
static u16int *video_mem = (void*) 0x100000;
static u16int c_base = 0;
static u16int cursor = 0;
static u8int attr = 0x0F;

void sync() {
	push_call(0, 0xB8000, (u32int) video_mem,  4000);

	outb(0x3D4, 15);
	outb(0x3D5, cursor & 0xFF);
	outb(0x3D4, 14);
	outb(0x3D5, cursor >> 8);
}

static void scroll() {
	int i;
	for (i = 0; i < 1920; i++) video_mem[i] = video_mem[i + 80];
    for (i = 1920; i < 2000; i++) video_mem[i] = 0x0F20;
	c_base -= 80;
	cursor -= 80;

	sync();
}

void cwrite(char c) {
	if (cursor >= 1999) scroll();
	switch (c) {
		case '\t': cursor = (cursor + 4) - (cursor % 4); break;
		case '\n': c_base = cursor = (cursor - cursor % 80) + 80; if (cursor>1920) scroll(); break;
		case '\r': cursor = (cursor - cursor % 80); c_base = cursor; break;
		case '\b': if (c_base < cursor) cursor--; video_mem[cursor] = 0x0F00; break;
		default: video_mem[cursor++] = (attr << 8) | c; break;
	}
}

static void swrite(char *s) {
	while (*s != '\0') cwrite(*s++);
}

void cleark() {
	int i;
	for (i = 0; i < 2000; i++) video_mem[i] = 0x0F20;
	c_base = 0;
	cursor = 0;

	push_call(0, 0xB8000, (u32int) video_mem, 4000);
}

void printk_list(char *fmt, u32int *argv) {
	char buffer[32];
	int i, n = 0;

//	pull_call(0, 0xB8000, (u32int) video_mem, 4000); // Sync up from vmem to get kernel output

	for (i = 0; fmt[i]; i++) {
		if (fmt[i] == '%') {
			switch (fmt[i+1]) {
				case 'd': swrite(itoa(argv[n++], buffer, 10)); break;
				case 'x': swrite(itoa(argv[n++], buffer, 16)); break;
				case 'c': cwrite((char) argv[n++] & 0xFF); break;
				case 's': swrite((char*) argv[n++]); break;
			}
			i++;
		}
		else cwrite(fmt[i]);
	}
	sync();
}

void printk(char *fmt, ...) {
	printk_list(fmt, (u32int*) &fmt + 1);
}

void colork(u8int color) {
	attr = color & 0x0F; // Any color, as long as it's black
}

void cursek(u8int x, u8int y) {
	if (x > 80) x = cursor % 80;
	if (y > 25) y = cursor / 80;
	cursor = c_base = (80 * y) + x;
	sync();
}*/
