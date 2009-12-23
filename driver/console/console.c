#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <khaos/kernel.h>
#include <khaos/driver.h>
#include <khaos/signal.h>
#include <driver/console.h>

#define VMEM 0xB8000
#define WIDTH 80
#define HEIGHT 25
#define TAB 4

static int console_init(uint16_t selector);
static int console_read(uintmax_t seek, size_t size, void *data);
static int console_write(uintmax_t seek, size_t size, void *data);
static void console_handler(uint32_t source, uint32_t args[4]);
static int console_halt(void);

static uint16_t vbuf[WIDTH * HEIGHT];
static uint16_t c_base = 0;
static uint16_t cursor = 0;
static uint16_t buffer = 0;

struct driver_interface console = {
	console_init,
	console_read,
	console_write,
	NULL,
	console_handler,
	1,
	console_halt,
};

static int console_init(uint16_t selector) {
	int i;
	selector = 0;

	for (i = 0; i < WIDTH * HEIGHT; i++) {
		vbuf[i] = 0x0F20;
	}

	return 0;
}

static int console_read(uintmax_t seek, size_t size, void *data) {
	size_t i;
	char *cdata = data;
	seek = 0;

	khsig_wait(0x20);

	for (i = 0; i < size && vbuf[buffer+i] & 0xFF; i++) {
		cdata[i] = vbuf[buffer + i] & 0xFF;
	}

	return i;
}

static void cwrite(char c) {
	uint16_t i;

	if (cursor >= WIDTH * HEIGHT) {
		for (i = 0; i < WIDTH * HEIGHT - WIDTH; i++) {
			vbuf[i] = vbuf[i + WIDTH];
		}

		for (i = WIDTH * HEIGHT - WIDTH; i < WIDTH * HEIGHT; i++) {
			vbuf[i] = 0x0F20;
		}
		
		c_base -= WIDTH;
		cursor -= WIDTH;
	}

	switch (c) {
		case '\0': 
			break;
		case '\t': 
			cursor = (cursor + TAB) - (cursor % TAB); break;
		case '\n': 
			buffer = c_base;
			vbuf[cursor] = 0x0000;
			c_base = cursor = (cursor - cursor % WIDTH) + 80; break;
		case '\r': 
			cursor = (cursor - cursor % WIDTH); 
			c_base = cursor; break;
		case '\b': 
			if (c_base < cursor) cursor--; 
			vbuf[cursor] = 0x0F00; break;
		default: 
			vbuf[cursor++] = (uint16_t) (c | 0x0F00); break;
	}

	outb(0x3D4, 14);
	outb(0x3D5, cursor >> 8);
	outb(0x3D4, 15);
	outb(0x3D5, cursor & 0xFF);
}

static int console_write(uintmax_t seek, size_t size, void *data) {
	size_t i;
	char *cdata = data;
	seek = 0;

	for (i = 0; i < size; i++) {
		cwrite(cdata[i]);
		buffer++;
	}

	push_call(0, VMEM, (uint32_t) vbuf, WIDTH * HEIGHT * sizeof(uint16_t));

	return size;
}

const char keymap[] = 
"\0\0331234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;\'`\0\\zxcvbnm,./\0*\0 ";

const char upkmap[] = 
"\0\033!@#$%^&*()_+\b\0QWERTYUIOP{}\n\0ASDFGHJKL:\"~\0|ZXCVBNM<>?\0*\0 ";

bool shift = false;

static void console_handler(uint32_t source, uint32_t args[4]) {
	size_t c = inb(0x60);
	uint32_t arg[4] = {0, 0, 0, 0};
	source = 0;
	args = NULL;

	if (c & 0x80) {
		if (keymap[c & 0x7F] == '\0') shift = false;
		return;
	}

	if (keymap[c & 0x7F] == '\0') shift = true;
	c = (size_t) ((shift) ? upkmap[c] : keymap[c]);

	khsig_send(info(0), 0x20, arg);

	cwrite(c);
	push_call(0, VMEM, (uint32_t) vbuf, WIDTH * HEIGHT * sizeof(uint16_t));
}

static int console_halt(void) {
	return 0;
}


