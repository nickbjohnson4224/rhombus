/* Copyright 2009 Nick Johnson */

#include <stdint.h>
#include <stdlib.h>
#include <flux.h>
#include <driver.h>
#include <config.h> 

void block() {
	_ctrl(CTRL_SCHED, CTRL_SCHED);
}

void unblock() {
	_ctrl(CTRL_NONE, CTRL_SCHED);
}

struct request *req_alloc(void) {
	return malloc(PAGESZ);
}

struct request *req_catch(uintptr_t grant) {
	void *vaddr = malloc(PAGESZ);
	_mmap((uintptr_t) vaddr, MMAP_READ | MMAP_WRITE | MMAP_FRAME, grant);
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

/*
uint16_t ntohs(uint8_t *s);
uint32_t ntohl(uint8_t *s);
void htons(uint8_t *d, uint16_t n);
void htonl(uint8_t *d, uint32_t n);

void req_decode(struct request *r, struct localrequest *l) {
	l->resource = 		ntohl(r->resource);
	l->transaction = 	ntohs(r->transaction);
	l->offset =			ntohl(r->fileoff);
	l->data = &((uint8_t*) r)[ntohs(r->dataoff)];
	l->datasize =		ntohs(r->datasize);
}

void req_encode(struct localrequest *l, struct request *r) {
	uint8_t *headerdata = (void*) ((uintptr_t) r + 4);
	uint16_t checksum = 0;
	size_t i;

	htonl(r->resource, l->resource);
	htons(r->datasize, l->datasize);
	htons(r->transaction, l->transaction);
	htons(r->dataoff, (uintptr_t) l->data - (uintptr_t) r);
	htons(r->format, 0x0000);
	htonl(r->fileoff, l->offset);

	for (i = 0; i < sizeof(struct request) - 4; i++) {
		checksum ^= headerdata[i];
	}

	htonl(r->checksum, checksum);
}

struct request *req_alloc() {
	return malloc(0x1000);
}

uint16_t ntohs(uint8_t *s) {
	return (s[0] << 8) | s[1];
}

uint32_t ntohl(uint8_t *s) {
	return (s[0] << 24) | (s[1] << 16) | (s[2] << 8) | s[3];
}

void htons(uint8_t *d, uint16_t n){
	d[1] = (n >> 0) & 0xFF;
	d[0] = (n >> 8) & 0xFF;
}

void htonl(uint8_t *d, uint32_t n){
	d[3] = (n >> 0 ) & 0xFF;
	d[2] = (n >> 8 ) & 0xFF;
	d[1] = (n >> 16) & 0xFF;
	d[0] = (n >> 24) & 0xFF;
} */
