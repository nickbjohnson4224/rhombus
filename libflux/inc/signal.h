#ifndef FLUX_SIGNAL_H
#define FLUX_SIGNAL_H

#include <flux/request.h>
#include <flux/arch.h>

#define MAXSIGNAL	32

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

int    fire(uint32_t target, uint32_t signal, req_t *req);

bool   signal_queue(uint32_t signal, bool value);
req_t *signal_recv(uint32_t signal);

typedef void (*sig_handler_t) (uint32_t caller, req_t *req);
void signal_register(uint32_t signal, sig_handler_t handler);

#endif/*FLUX_SIGNAL_H*/
