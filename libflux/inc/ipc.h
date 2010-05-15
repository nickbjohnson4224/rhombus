#ifndef FLUX_IPC_H
#define FLUX_IPC_H

#include <flux/arch.h>
#include <flux/packet.h>

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
#define PORT_SYNC	32
#define PORT_PING	33

typedef void (*event_t) (uint32_t source, struct packet *packet);

uint32_t       send (uint32_t port, uint32_t target, struct packet *packet);
struct packet *recv (uint32_t port);
struct packet *recvs(uint32_t port, uint32_t source);
struct packet *wait (uint32_t port);
struct packet *waits(uint32_t port, uint32_t source);
event_t        event(uint32_t port, event_t handler);

#endif/*FLUX_IPC_H*/
