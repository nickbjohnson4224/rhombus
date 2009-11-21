#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <khaos/kernel.h>
#include <khaos/driver.h>
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

static uint16_t *vbuf = NULL;
static struct charcell {
	char ch;
	struct charcell *next;
} *kbuf = NULL;

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

	vbuf = calloc(sizeof(uint16_t), WIDTH * HEIGHT);
	if (!vbuf) return 1;

	for (i = 0; i < WIDTH * HEIGHT; i++) {
		vbuf[i] = 0x0F20;
	}

	return 0;
}

static int console_read(uintmax_t seek, size_t size, void *data) {
	struct charcell *cc = kbuf, *ct;
	size_t i;
	char *cdata = data;
	seek = 0;

	for (i = 0; cc && i < size; i++) {
		cdata[i] = cc->ch;
		ct = cc;
		cc = cc->next;
		free(ct);
	}

	return i;
}

static void cwrite(char c) {
	static uint16_t cursor = 0;
	static uint16_t c_base = 0;
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
}

static int console_write(uintmax_t seek, size_t size, void *data) {
	size_t i;
	char *cdata = data;
	seek = 0;

	for (i = 0; i < size; i++) {
		cwrite(cdata[i]);
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
	struct charcell *cc;
	source = 0;
	args = NULL;

	if (c & 0x80) {
		if (keymap[c & 0x7F] == '\0') shift = false;
		return;
	}

	if (keymap[c & 0x7F] == '\0') shift = true;
	c = (size_t) ((shift) ? upkmap[c] : keymap[c]);

	cc = malloc(sizeof(struct charcell));
	cc->ch = c;
	cc->next = kbuf;
	kbuf = cc;
}

static int console_halt(void) {
	free(vbuf);
	return 0;
}

/*
static uint16_t video_buf[2000];
static uint16_t *video_mem = (void*) 0xB8000;
static int c_base = 0;
static int cursor = 0;
static uint8_t attr = 0x0F;

void sclear(void) {
}

static void scroll(void) {
}


void nwrite(int n, int b) {
	const char d[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char buffer[10];
	int i = 0;

	if (!n) {
		swrite("0");
		return;
	}

	while (n) {
		buffer[i++] = d[n % b];
		n /= b;
	}

	for (; i > 0; i--) {
		cwrite(buffer[i-1]);
	}

	swrite("");
}

void swrite(const char *s) {
	while (*s != '\0') cwrite(*s++);
	push_call(0, (uint32_t) video_mem, (uint32_t) video_buf, 4000);
	outb(0x3D4, 15);
	outb(0x3D5, cursor & 0xFF);
	outb(0x3D4, 14);
	outb(0x3D5, cursor >> 8);
}

void curse(int y, int x) {
	c_base = cursor = (y * 80) + x;
	outb(0x3D4, 15);
	outb(0x3D5, cursor & 0xFF);
	outb(0x3D4, 14);
	outb(0x3D5, cursor >> 8);
}

void gets(char *buf) {
	int i;

	for (i = c_base; i < cursor && i - c_base < 10; i++) {
		buf[i] = video_buf[i] & 0xFF;
	}

	buf[i] = '\0';
}

void print_bootsplash() {
	sclear();

	swrite("\n\n\n\
\t\t\t\t\t\t\t           '^a,\n\
\t\t\t\t\t\t\t        ,.    'b.\n\
\t\t\t\t\t\t\t      .d'       b.\n\
\t\t\t\t\t\t\t      S:        a:\n\
\t\t\t\t\t\t\t      'q,       p'\n\
\t\t\t\t\t\t\t        \"'    .p'\n\
\t\t\t\t\t\t\t           .,a'\n\n\
\t\t\t\t\t _  _   _   _   _____   _____   _____ \n\
\t\t\t\t\t| |/ / | |_| | |___  | |  _  | |  ___|\n\
\t\t\t\t\t|   <  |  _  | |  _  | | |_| | |___  |\n\
\t\t\t\t\t|_|\\_\\ |_| |_| |_____| |_____| |_____|\n\
\t\t\t\t\t          -= Version 0.1a =-\n\n\
\t\t\t\t\t   [                              ]\n");
}

void update_progress(const char *message) {
	static int xpos = 24;
	curse(17, xpos);
	swrite("*");
	if (xpos < 50) xpos ++;
	curse(18, 24);
	swrite(message);
	swrite("                \b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
}
*/
