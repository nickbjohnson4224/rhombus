// Copyright 2009 Nick Johnson

#include <lib.h>
#include <trap.h>

#define L_SHIFT 0x2A
#define R_SHIFT 0x36

#define ALT 0x38
#define CTRL 0x1D

char keymap[128] = "\0\0331234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;\'`\0\\zxcvbnm,./\0*\0 ";
char upkmap[128] = "\0\033!@#$%^&*()_+\b\0QWERTYUIOP{}\n\0ASDFGHJKL:\"~\0|ZXCVBNM<>?\0*\0 ";
char kstate[128];

void *kb_handler(image_t *t) {
	u32int k = inb(0x60);
	if (k & 0x80) {
		kstate[k & 0x2F] = 0;
		return t;
	}
	else kstate[k] = 1;
	if (kstate[L_SHIFT] || kstate[R_SHIFT]) k = upkmap[k];
	else k = keymap[k];

	if (k) printk("%c", k);
	return t;
}
