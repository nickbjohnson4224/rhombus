/* Copyright 2010 Nick Johnson */

#ifndef REQUEST_H
#define REQUEST_H

#include <flux/arch.h>

/*** Flux Standard Request Protocol ***/

#define REQSZ (PAGESZ - 512)
#define STDOFF 512

/* Request header */
typedef struct request {
	uint32_t checksum;			/* Checksum (bit parity) */
	uint32_t resource;			/* Resource ID */
	uint16_t datasize;			/* Size of request data */
	uint16_t transid;			/* Transaction ID */
	uint16_t dataoff;			/* Offset of request data */
	uint16_t format;			/* Header format */
	uint64_t fileoff;			/* File offset */
	uint64_t fileoff_ext;		/* Extended file offset */
	uint8_t  reqdata[];			/* Request data area */
} __attribute__ ((packed)) req_t;

#define REQ_READ  0
#define REQ_WRITE 1
#define REQ_ERROR 2

req_t   *req_cksum(req_t *req);	/* Checksum request */
bool     req_check(req_t *req);	/* Check request for validity */

bool     req_setbuf(req_t *req, uint16_t offset, uint16_t size);
uint8_t *req_getbuf(req_t *req);

req_t   *ralloc(void);
void     rfree(req_t *req);

#endif
