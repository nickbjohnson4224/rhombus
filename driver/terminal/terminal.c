/* Copyright 2009, 2010 Nick Johnson */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <flux/flux.h>

#include <driver/terminal.h>

#define VMEM 0xB8000
#define WIDTH 80
#define HEIGHT 25
#define TAB 4

static void terminal_init (device_t selector);
static void terminal_halt (void);
static void terminal_write(uint32_t caller, req_t *req);

static uint16_t *vbuf;
static uint16_t c_base = 0;
static uint16_t cursor = 0;
static uint16_t buffer = 0;
static void char_write(char c);

struct driver_interface terminal = {
	terminal_init,
	terminal_halt,

	NULL,
	0,
};

static void terminal_init(device_t selector) {
	size_t i;

	vbuf = malloc(0x1000);
	emap(vbuf, VMEM, PROT_READ | PROT_WRITE);

	for (i = 0; i < WIDTH * HEIGHT; i++) {
		vbuf[i] = 0x0F00 | ' ';
	}

	sigregister(SIG_WRITE, terminal_write);
}

static void terminal_halt(void) {
	return;
}

static void terminal_write(uint32_t caller, struct request *req) {
	size_t i;
	char *buffer;

	if (!req_check(req)) {
		if (!req) req = ralloc();
		req->format = REQ_ERROR;
		tail(caller, SIG_REPLY, req);
	}

	buffer = (void*) req_getbuf(req);

	sigblock(true, VSIG_REQ);

	for (i = 0; i < req->datasize; i++) {
		char_write(buffer[i]);
	}

	outb(0x3D4, 14);
	outb(0x3D5, cursor >> 8);
	outb(0x3D4, 15);
	outb(0x3D5, cursor & 0xFF);

	sigblock(false, VSIG_REQ);

	req->datasize = i;
	req->format = REQ_READ;
	tail(caller, SIG_REPLY, req_cksum(req));
}

static void char_write(char c) {
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
			cursor = (cursor + TAB) - (cursor % TAB); 
			break;
		case '\n': 
			buffer = c_base;
			vbuf[cursor] = 0x0000;
			c_base = cursor = (cursor - cursor % WIDTH) + 80; 
			break;
		case '\r': 
			cursor = (cursor - cursor % WIDTH); 
			c_base = cursor; 
			break;
		case '\b': 
			if (c_base < cursor) cursor--; 
			vbuf[cursor] = 0x0F00; 
			break;
		default: 
			vbuf[cursor++] = (uint16_t) (c | 0x0F00); 
			break;
	}

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
}
