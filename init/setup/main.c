#include <lib.h>
#include <stdint.h>
#include <kernel.h>
#include <driver.h>

void death() {
	sret_call(3);
}

void segfault() {
	eout_call("Page Fault\n");
	exit_call(1);
}

void gepfault() {
	eout_call("General Protection Fault\n");
	exit_call(1);
}

void imgfault() {
	eout_call("Image Stack Overflow (DoS)");
	exit_call(1);
}


static uint16_t video_buf[4000];
static uint16_t *video_mem = (void*) 0xB8000;
static int c_base = 0;
static int cursor = 0;
static uint8_t attr = 0x0F;

static void scroll(void) {
	int i;
	for (i = 0; i < 1920; i++) video_buf[i] = video_buf[i + 80];
    for (i = 1920; i < 2000; i++) video_buf[i] = 0x0F20;
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
		case '\b': if (c_base < cursor) cursor--; video_buf[cursor] = 0x0F00; break;
		default: video_buf[cursor++] = (uint16_t) ((attr << 8) | c); break;
	}
	push_call(0, (uint32_t) video_mem, (uint32_t) video_buf, 4000);
	outb(0x3D4, 15);
	outb(0x3D5, cursor & 0xFF);
	outb(0x3D4, 14);
	outb(0x3D5, cursor >> 8);
}

void swrite(char *s) {
	while (*s != '\0') cwrite(*s++);
}

u8int keymap[128] = "\0\0331234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;\'`\0\\zxcvbnm,./\0*\0 ";
u8int upkmap[128] = "\0\033!@#$%^&*()_+\b\0QWERTYUIOP{}\n\0ASDFGHJKL:\"~\0|ZXCVBNM<>?\0*\0 ";
u8int shift = 0;
void kbhandle() {
	char c = inb(0x60);
	if (c & 0x80) {
		if (keymap[(int) c & 0x7F] == '\0') shift = 0;
		sret_call(3);
	}
	if (keymap[(int) c & 0x7F] == '\0') shift = 1;
	c = (shift) ? upkmap[(int) c] : keymap[(int) c];
	cwrite(c);
	sret_call(3);
}

char buffer2[100];
int init() {

	rsig_call(0, (uint32_t) gepfault);
	rsig_call(2, (uint32_t) segfault);
	rsig_call(5, (uint32_t) imgfault);
	rsig_call(3, (uint32_t) kbhandle);
	rirq_call(1);

	for(;;);
	return 0;
}
