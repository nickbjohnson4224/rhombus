/* Copyright 2009 Nick Johnson */

#include <stdint.h>
#include <stdlib.h>
#include <flux.h>
#include <driver.h>
#include <config.h>
#include <mmap.h>
#include "libc.h"

void block() {
	_ctrl(CTRL_SCHED, CTRL_SCHED);
}

void unblock() {
	_ctrl(CTRL_NONE, CTRL_SCHED);
}

struct request *req_alloc(void) {
	return _heap_req_alloc();
}

void req_free(struct request *r) {
	_heap_req_free(r);
}

struct request *req_catch(void *grant) {
	void *vaddr = _heap_req_alloc();
	emap(vaddr, (uintptr_t) grant, PROT_READ | PROT_WRITE);
	return vaddr;
}

struct request *req_checksum(struct request *r) {
	uint32_t *reqflat = (void*) r;
	size_t i;
	uint32_t checksum = 0;

	for (i = 1; i < sizeof(struct request) / sizeof(uint32_t); i++) {
		checksum ^= reqflat[i];
	}

	r->checksum = checksum;
	return r;
}

bool req_check(struct request *r) {
	uint32_t *reqflat = (void*) r;
	uint32_t checksum = 0;
	size_t i;

	for (i = 0; i < sizeof(struct request) / sizeof(uint32_t); i++) {
		checksum ^= reqflat[i];
	}

	return ((checksum) ? false : true);
}

uint16_t dev_getdevice(device_t dev) {
	uint8_t type = (dev >> 24) & 0x7F;

	switch (type) {
	case 0:
	case 1:
	default: return 0;
	}
}

uint16_t dev_getiobase(device_t dev, int bar) {
	uint8_t type = (dev >> 24) & 0x7F;

	switch (type) {
	case 0:
		switch ((dev >> 8) & 0xFF) {
		case 0: return (bar) ? 0x1F0 : 0x3F6;
		case 1: return (bar) ? 0x170 : 0x376;
		case 2: return (bar) ? 0x1E8 : 0x3E6;
		case 3: return (bar) ? 0x168 : 0x366;
		case 4: return 0x60;
		}
	case 1:
	default: return 0;
	}
}

uint16_t dev_getiolimit(device_t dev, int bar) {
	uint8_t type = (dev >> 24) & 0x7F;

	switch (type) {
	case 0:
		switch ((dev >> 8) & 0xFF) {
		case 0: return (bar) ? 0x1F7 : 0x3F6;
		case 1: return (bar) ? 0x177 : 0x376;
		case 2: return (bar) ? 0x1E7 : 0x3E6;
		case 3: return (bar) ? 0x167 : 0x366;
		case 4: return 0x60;
		}
	case 1:
	default: return 0;
	}
}

uint16_t dev_getirqnum(device_t dev) {
	uint8_t type = (dev >> 24) & 0x7F;

	switch (type) {
	case 0:
		switch ((dev >> 8) & 0xFF) {
		case 0: return 14;
		case 1: return 15;
		case 4: return 1;
		case 6: return 6;
		}
	case 1:
	default: return 0;
	}
}
