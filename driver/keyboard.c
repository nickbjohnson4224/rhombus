/* Copyright 2009 Nick Johnson */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <flux.h>
#include <mmap.h>
#include <driver.h>

#include <driver/keyboard.h>

static char buffer[1024];
static size_t buffer_top;

static int keyboard_init(device_t selector);
static int keyboard_sleep(void);
static int keyboard_halt(void);

static int keyboard_read (struct request *r, callback_t cb);
static int keyboard_write(struct request *r, callback_t cb);
static int keyboard_ctrl (struct request *r, callback_t cb);
static int keyboard_info (struct request *r, callback_t cb);

static void keyboard_work(void);
static void keyboard_handler(void);

static const char dnkmap[] = 
"\0\0331234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;\'`\0\\zxcvbnm,./\0*\0 ";
static const char upkmap[] = 
"\0\033!@#$%^&*()_+\b\0QWERTYUIOP{}\n\0ASDFGHJKL:\"~\0|ZXCVBNM<>?\0*\0 ";

static bool shift = false;

struct driver_interface keyboard = {
	keyboard_init,
	keyboard_sleep,
	keyboard_halt,

	keyboard_read,
	keyboard_write,
	keyboard_ctrl,
	keyboard_info,

	keyboard_work,
	0,

	keyboard_handler,
	1,
};

static int keyboard_init(device_t selector) {
	return DRV_DONE;
}

static int keyboard_sleep(void) {
	return DRV_DONE;
}

static int keyboard_halt(void) {
	return DRV_DONE;
}

static int keyboard_read (struct request *r, callback_t cb) {

	sigblock();

	if (r->datasize > buffer_top) {
		r->datasize = buffer_top;
	}

	memcpy(r->reqdata, buffer, sizeof(char) * r->datasize);
	
	if (r->datasize == buffer_top) {
		buffer_top = 0;
	}

	sigunblock();

	if (cb) cb(r);

	return DRV_DONE;
}

static int keyboard_write(struct request *r, callback_t cb) {
	return DRV_ERROR;
}

static int keyboard_ctrl (struct request *r, callback_t cb) {
	return DRV_ERROR;
}

static int keyboard_info (struct request *r, callback_t cb) {
	return DRV_ERROR;
}

static void keyboard_work(void) {
	return;
}

static void keyboard_handler(void) {
	extern size_t console_write(char *buffer, size_t length);
	uint8_t scan = inb(0x60);

	if (scan & 0x80) {
		if (dnkmap[scan & 0x7F] == '\0') {
			shift = false;
		}
		return;
	}

	if (dnkmap[scan & 0x7F] == '\0') {
		shift = true;
	}

	sigblock();

	buffer[buffer_top] = ((shift) ? upkmap[scan] : dnkmap[scan]);
	console_write(&buffer[buffer_top], 1);
	buffer_top++;

	sigunblock();
}
