#ifndef FLUX_IPC_H
#define FLUX_IPC_H

#include <flux/request.h>
#include <flux/arch.h>

#define PORT_FAULT	0
#define PORT_IRQ	3
#define PORT_FLOAT	6
#define PORT_DEATH	7

#define PORT_READ	16
#define PORT_WRITE	17
#define PORT_INFO	18
#define PORT_CTRL	19
#define PORT_QUERY	20
#define PORT_REPLY	31

typedef void (*event_t) (uint32_t source, req_t *packet);

uint32_t send (uint32_t port, uint32_t target, req_t *packet);
req_t   *recv (uint32_t port);
req_t   *recvs(uint32_t port, uint32_t source);
req_t   *wait (uint32_t port);
req_t   *waits(uint32_t port, uint32_t source);
event_t  event(uint32_t port, event_t handler);

#endif/*FLUX_IPC_H*/
