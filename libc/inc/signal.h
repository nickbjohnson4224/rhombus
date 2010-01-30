#ifndef SIGNAL_H
#define SIGNAL_H

#include <stdint.h>
#include <config.h>
#include <stdbool.h>
#include <flux.h>

#define MAXSIGNAL 256

/*** Flux Request Protocol ***/

#define REQSZ (PAGESZ - 32)

struct request {
	uint32_t checksum;			/* Checksum (bit parity) */
	uint32_t resource;			/* Resource ID */
	uint16_t datasize;			/* Size of request data */
	uint16_t transid;			/* Transaction ID */
	uint16_t dataoff;			/* Offset of request data */
	uint16_t format;			/* Header format */
	uint32_t fileoff[4];		/* File offset */
	uint8_t  reqdata[REQSZ];	/* Request data area */
} __attribute__ ((packed));

struct request *req_alloc(void);
void			req_free(struct request *r);
struct request *req_catch(void *grant);
struct request *req_checksum(struct request *r);
bool            req_check(struct request *r);

#define REQ_READ 0
#define REQ_WRITE 1

/*** Signals ***/

void sigblock(void);
void sigunblock(void);

int fire(uint32_t target, uint16_t signal, struct request *req);
void tail(uint32_t target, uint16_t signal, struct request *req);

typedef void (*signal_handler_t) (uint32_t caller, struct request *req);
void sigregister(uint16_t signal, signal_handler_t handler);

void sighold(uint16_t signal);	/* Hold signal */
void sigfree(uint16_t signal);	/* Stop holding signal */
struct request *sigpull(uint16_t signal);	/* Get held signal */

#endif
