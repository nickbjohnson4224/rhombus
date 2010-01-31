/* Copyright 2009, 2010 Nick Johnson */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <flux.h>
#include <mmap.h>
#include <driver.h>

#include <driver/keyboard.h>

static void keyboard_init(device_t selector);
static void keyboard_halt(void);

static void keyboard_read(uint32_t caller, struct request *req);
static void keyboard_hand(uint32_t caller, struct request *req);

static const char dnkmap[] = 
"\0\0331234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;\'`\0\\zxcvbnm,./\0*\0 ";
static const char upkmap[] = 
"\0\033!@#$%^&*()_+\b\0QWERTYUIOP{}\n\0ASDFGHJKL:\"~\0|ZXCVBNM<>?\0*\0 ";

static volatile char buffer[1024];
static volatile size_t buffer_top;
static volatile bool shift = false;

struct driver_interface keyboard = {
	keyboard_init,
	keyboard_halt,
	NULL,
	0,
};

static void keyboard_init(device_t selector) {
	rirq(1);
	sigregister(SSIG_IRQ, keyboard_hand);
	sigregister(SIG_READ, keyboard_read);
}

static void keyboard_halt(void) {
	return;
}

static void keyboard_read (uint32_t caller, struct request *req) {

	if (!req || !req_check(req)) {
		if (req) req_free(req);
		tail(caller, SIG_ERROR, NULL);
	}

	while (!buffer_top) sleep();

	sigblock();

	if (req->datasize > buffer_top) {
		req->datasize = buffer_top;
	}

	req->dataoff = STDOFF;
	memcpy(&req->reqdata[req->dataoff - HDRSZ], (void*) buffer, req->datasize);
	
	if (req->datasize == buffer_top) {
		buffer_top = 0;
	}

	sigunblock();

	req->format = REQ_WRITE;
	tail(caller, SIG_REPLY, req_checksum(req));
}

static void keyboard_hand(uint32_t caller, struct request *req) {
	uint8_t scan;
	
	sigblock();

	scan = inb(0x60);

	if (scan & 0x80) {
		if (dnkmap[scan & 0x7F] == '\0') {
			shift = false;
		}
	}

	else if (dnkmap[scan & 0x7F] == '\0') {
		shift = true;
	}

	else {
		buffer[buffer_top] = ((shift) ? upkmap[scan] : dnkmap[scan]);
		buffer_top++;

		sigunblock();
		fwrite((char*) &buffer[buffer_top-1], 1, 1, stdout);
		sigblock();
	}

	sigunblock();
	return;
}
