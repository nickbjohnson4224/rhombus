/* Copyright 2009, 2010 Nick Johnson */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <flux.h>
#include <mmap.h>
#include <driver.h>

#include <driver/keyboard.h>

static void keyboard_init(device_t selector);
static void keyboard_halt(void);

static void keyboard_read(uint32_t caller, void *grant);
static void keyboard_hand(uint32_t caller, void *grant);

static const char dnkmap[] = 
"\0\0331234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;\'`\0\\zxcvbnm,./\0*\0 ";
static const char upkmap[] = 
"\0\033!@#$%^&*()_+\b\0QWERTYUIOP{}\n\0ASDFGHJKL:\"~\0|ZXCVBNM<>?\0*\0 ";

static char buffer[1024];
static size_t buffer_top;
static bool shift = false;

struct driver_interface keyboard = {
	keyboard_init,
	keyboard_halt,

	NULL,
	0,
};

static void keyboard_init(device_t selector) {
	rirq(0);
	sigregister(SSIG_IRQ, keyboard_hand);
	sigregister(SIG_READ, keyboard_read);
}

static void keyboard_halt(void) {
	return;
}

static void keyboard_read (uint32_t caller, void *grant) {
	struct request *r = req_catch(grant);

	if (!req_check(r)) return;

	sigblock();

	if (r->datasize > buffer_top) {
		r->datasize = buffer_top;
	}

	memcpy(r->reqdata, buffer, sizeof(char) * r->datasize);
	
	if (r->datasize == buffer_top) {
		buffer_top = 0;
	}

	sigunblock();

	r->format = REQ_WRITE;
	fire(caller, SIG_REPLY, r);
	free(r);
}

static void keyboard_hand(uint32_t caller, void *grant) {
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
	buffer_top++;

	sigunblock();
}
