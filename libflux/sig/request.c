/* Copyright 2010 Nick Johnson */

#include <flux/arch.h>
#include <flux/request.h>

req_t *req_cksum(req_t *r) {
	uint32_t *reqflat = (void*) r;
	size_t i;
	uint32_t checksum = 0;

	for (i = 1; i < sizeof(struct request) / sizeof(uint32_t); i++) {
		checksum ^= reqflat[i];
	}

	r->checksum = checksum;
	return r;
}

bool req_check(req_t *r) {
	uint32_t *reqflat = (void*) r;
	uint32_t checksum = 0;
	size_t i;

	if (!r) {
		return false;
	}

	for (i = 0; i < sizeof(struct request) / sizeof(uint32_t); i++) {
		checksum ^= reqflat[i];
	}

	if (checksum) {
		return false;
	}

	if (r->format == REQ_WRITE) {
		if (r->dataoff + r->datasize > PAGESZ || r->dataoff < 32) {
			return false;
		}
	}

	return true;
}

bool req_setbuf(req_t *r, uint16_t offset, uint16_t size) {
	if (!req_check(r) || offset < 32 || offset + size > PAGESZ) {
		return false;
	}

	r->dataoff = offset;
	r->datasize = size;

	return true;
}

uint8_t *req_getbuf(req_t *r) {
	return &r->reqdata[r->dataoff - sizeof(req_t)];
}
