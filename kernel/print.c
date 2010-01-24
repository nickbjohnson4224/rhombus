/* Copyright 2009, 2010 Nick Johnson */

#include <lib.h>
#include <mem.h>

static uint16_t *video_mem = (void*) (KSPACE + 0xB8000);
static int c_base = 0;
static int cursor = 0;
static uint8_t attr = 0x0F;

static void scroll(void) {
	int i;
	for (i = 0; i < 1920; i++) video_mem[i] = video_mem[i + 80];
    for (i = 1920; i < 2000; i++) video_mem[i] = 0x0F20;
	c_base -= 80;
	cursor -= 80;
}

static void cwrite(char c) {
	if (cursor >= 1999) scroll();
	switch (c) {
		case '\t': cursor = (cursor + 4) - (cursor % 4); break;
		case '\n': c_base = cursor = (cursor - cursor % 80) + 80; if (cursor>1920) scroll(); break;
		case '\r': cursor = (cursor - cursor % 80); c_base = cursor; break;
		case '\b': if (c_base < cursor) cursor--; video_mem[cursor] = 0x0F00; break;
		default: video_mem[cursor++] = (uint16_t) ((attr << 8) | c); break;
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
}

static void printk_list(const char *fmt, uint32_t *argv) {
	char buffer[32];
	int i, n = 0;
	for (i = 0; fmt[i]; i++) {
		if (fmt[i] == '%') {
			switch (fmt[i+1]) {
				case 'd': swrite(itoa(argv[n++], buffer, 10)); break;
				case 'x': swrite(itoa(argv[n++], buffer, 16)); break;
				case 'c': cwrite((char) argv[n++]); break;
				case 's': swrite((char*) argv[n++]); break;
				case '%': cwrite('%'); break;
			}
			i++;
		}
		else {
			cwrite(fmt[i]);
		}
	}
	outb(0x3D4, 14);
	outb(0x3D5, (uint8_t) (cursor >> 8));
	outb(0x3D4, 15);
	outb(0x3D5, (uint8_t) (cursor & 0xFF));
}

void printk(const char *fmt, ...) {
	printk_list(fmt, (void*) ((uintptr_t) &fmt + sizeof(const char*) * 4));
}

void colork(uint8_t color) {
	attr = (uint8_t) (color & 0x0F); /* Any color, as long as it's black */
}

void cursek(int8_t x, int8_t y) {
	if (x < 0) x = (int8_t) (cursor % 80);
	if (y < 0) y = (int8_t) (cursor / 80);
	cursor = c_base = (80 * y) + x;
}
