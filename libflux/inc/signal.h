#ifndef FLUX_SIGNAL_H
#define FLUX_SIGNAL_H

#include <flux/request.h>
#include <flux/arch.h>

#define MAXSIGNAL	32

void sigblock(bool v, uint8_t signal);	/* (Dis)allow signals */

int  fire(uint32_t target, uint8_t signal, req_t *req);
void tail(uint32_t target, uint8_t signal, req_t *req);

typedef void (*sig_handler_t) (uint32_t caller, req_t *req);
void sigregister(uint16_t signal, sig_handler_t handler);

void   sighold(uint16_t signal);			/* Hold signal */
void   sigfree(uint16_t signal);			/* Stop holding signal */
req_t *sigpull(uint16_t signal);			/* Get held signal request */
void   sigpush(uint16_t signal, req_t *r);	/* Re-hold signal request */

#define SSIG_FAULT	0x00
#define SSIG_ENTER	0x01
#define SSIG_PAGE	0x02
#define SSIG_IRQ	0x03
#define SSIG_KILL	0x04
#define SSIG_IMAGE	0x05
#define SSIG_FLOAT	0x06
#define SSIG_DEATH	0x07

#define SIG_READ	0x10
#define SIG_WRITE	0x11
#define SIG_INFO	0x12
#define SIG_CTRL	0x13
#define SIG_REPLY	0x1F

#define VSIG_ALL	0xFF
#define VSIG_REQ	0xFE

#endif
