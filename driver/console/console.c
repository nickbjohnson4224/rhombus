#include <stdint.h>
#include <khaos/kernel.h>
#include <khaos/driver.h>
#include <driver/console.h>

static uint16_t video_buf[2000];
static uint16_t *video_mem = (void*) 0xB8000;
static int c_base = 0;
static int cursor = 0;
static uint8_t attr = 0x0F;

void sclear(void) {
	int i;
	for (i = 0; i < 2000; i++) video_buf[i] = 0x20 | (attr << 8);
}

static void scroll(void) {
	int i;
	for (i = 0; i < 1920; i++) video_buf[i] = video_buf[i + 80];
    for (i = 1920; i < 2000; i++) video_buf[i] = 0x20 | (attr << 8);
	c_base -= 80;
	cursor -= 80;
}

void cwrite(char c) {
	if (cursor >= 1999) scroll();
	switch (c) {
		case '\0': break;
		case '\t': cursor = (cursor + 4) - (cursor % 4); break;
		case '\n': c_base = cursor = (cursor - cursor % 80) + 80; if (cursor>1920) scroll(); break;
		case '\r': cursor = (cursor - cursor % 80); c_base = cursor; break;
		case '\b': if (c_base < cursor) cursor--; video_buf[cursor] = 0x00 | (attr << 8); break;
		default: video_buf[cursor++] = (uint16_t) ((attr << 8) | c); break;
	}
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
\t\t\t\t\t|  <   |  _  | |  _  | | |_| | |___  |\n\
\t\t\t\t\t|_|\\_\\ |_| |_| |_____| |_____| |_____|\n\
\t\t\t\t\t         -= Version 0.1a =-\n\n\
\t\t\t\t\t[                                    ]\n");
}

void update_progress(const char *message) {
	static int xpos = 21;
	curse(17, xpos);
	swrite("*");
	if (xpos < 56) xpos ++;
	curse(18, 21);
	swrite(message);
	swrite("                \b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
}

uint8_t keymap[128] = "\0\0331234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;\'`\0\\zxcvbnm,./\0*\0 ";
uint8_t upkmap[128] = "\0\033!@#$%^&*()_+\b\0QWERTYUIOP{}\n\0ASDFGHJKL:\"~\0|ZXCVBNM<>?\0*\0 ";
uint8_t shift = 0;
void kbhandle() {
	char c = inb(0x60);
	char m[2] = " ";
	if (c & 0x80) {
		if (keymap[(int) c & 0x7F] == '\0') shift = 0;
		sret_call(3);
	}
	if (keymap[(int) c & 0x7F] == '\0') shift = 1;
	c = (shift) ? upkmap[(int) c] : keymap[(int) c];
	m[0] = c;
	swrite(m);
	sret_call(3);
}
