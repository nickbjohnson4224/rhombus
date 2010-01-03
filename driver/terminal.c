/* Copyright 2009 Nick Johnson */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <flux.h>
#include <mmap.h>
#include <driver.h>

#include <driver/console.h>

#define VMEM 0xB8000
#define WIDTH 80
#define HEIGHT 25
#define TAB 4

static int terminal_init(device_t selector);
static int terminal_sleep(void);
static int terminal_halt(void);

static int terminal_read (struct request *r, callback_t cb);
static int terminal_write(struct request *r, callback_t cb);
static int terminal_ctrl (struct request *r, callback_t cb);
static int terminal_info (struct request *r, callback_t cb);

static void terminal_work(void);
static void terminal_handler(void);

static device_t dev;
static uint16_t *vbuf;
static uint16_t c_base = 0;
static uint16_t cursor = 0;
static uint16_t buffer = 0;
static void char_write(char c);

struct driver_interface terminal = {
	terminal_init,
	terminal_sleep,
	terminal_halt,

	terminal_read,
	terminal_write,
	terminal_ctrl,
	terminal_info,

	terminal_work,
	0,

	terminal_handler,
	-1,
};

static int terminal_init(device_t selector) {
	int i;

	dev = selector;

	vbuf = malloc(0x1000);
	emap(vbuf, VMEM, PROT_READ | PROT_WRITE);

	for (i = 0; i < WIDTH * HEIGHT; i++) {
		vbuf[i] = 0x0F20;
	}

	return DRV_DONE;
}

static int terminal_sleep(void) {
	return DRV_ERROR;
}

static int terminal_halt(void) {
	return DRV_ERROR;
}

static int terminal_read (struct request *r, callback_t cb) {
	return DRV_ERROR;
}

static int terminal_write(struct request *r, callback_t cb) {
	struct localrequest l;
	char *cdata;
	int *idata;
	size_t i;

	req_decode(r, &l);
	cdata = l.data;
	idata = l.data;

	for (i = 0; i < l.datasize; i++) {
		char_write(cdata[i]);
	}

	outb(0x3D4, 14);
	outb(0x3D5, cursor >> 8);
	outb(0x3D4, 15);
	outb(0x3D5, cursor & 0xFF);

	l.datasize = 4;
	*idata = i;
	req_encode(&l, r);

	if (cb) cb(r);

	return DRV_DONE;
}

static int terminal_ctrl (struct request *r, callback_t cb) {
	return DRV_ERROR;
}

static int terminal_info (struct request *r, callback_t cb) {
	return DRV_ERROR;
}

static void terminal_work(void) {
	return;
}

static void terminal_handler(void) {
	return;
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


