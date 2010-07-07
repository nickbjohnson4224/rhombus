#ifndef FLUX_IPC_H
#define FLUX_IPC_H

#include <flux/arch.h>

/*** Flux Packet Protocol (FPP) ***/

#define PACKET_MAXDATA		(PAGESZ - 58)
#define PACKET_PROTOCOL		3	/* protocol version */

struct packet {

	/* general information */
	uint16_t identity;
	uint16_t protocol;

	/* fragment information */
	uint16_t fragment_index;
	uint16_t fragment_count;

	/* data buffer information */
	uint32_t data_length;
	uint32_t data_offset;

	/* source routing information */
	uint32_t source_pid;
	uint64_t source_inode;

	/* target routing information */
	uint32_t target_pid;
	uint64_t target_inode;

	/* target file offset */
	uint64_t offset;
	uint64_t offset_ext;

} __attribute__ ((packed));

bool  packet_setbuf(struct packet **packet, uint32_t length);
void *packet_getbuf(struct packet *packet);

void *packet_alloc(uint32_t size);
void  packet_free (struct packet *packet);

/*** Standard Port Numbers ***/

#define PORT_FAULT	0
#define PORT_IRQ	3
#define PORT_FLOAT	6
#define PORT_DEATH	7

#define PORT_READ	16
#define PORT_WRITE	17
#define PORT_CALL	42
#define PORT_DICT	18
#define PORT_INFO	18
#define PORT_CTRL	19
#define PORT_QUERY	20

#define PORT_REPLY	31
#define PORT_SYNC	32
#define PORT_PING	33
#define PORT_ERROR	34

/* queueing */
struct message {
	struct message *next;
	struct message *prev;
	struct packet *packet;
	uint32_t source;
};

extern struct   message msg_queue[256];
extern uint32_t m_msg_queue[256];

void           stash(struct packet *packet, uint8_t port, uint32_t source);

/* send */
int            send (uint8_t port, uint32_t target, struct packet *packet);

/* recv family - asynchronous */
struct packet *recv (uint8_t port);
struct packet *recvs(uint8_t port, uint32_t source);
struct packet *recvn(uint8_t port, uint32_t source, uint64_t inode);
struct packet *recvi(uint8_t port, uint32_t source, uint16_t id);
struct packet *recvf(uint8_t port, uint32_t source, uint16_t id, uint16_t frag);

/* wait family - synchronous */
struct packet *wait (uint8_t port);
struct packet *waits(uint8_t port, uint32_t source);
struct packet *waitn(uint8_t port, uint32_t source, uint64_t inode);
struct packet *waiti(uint8_t port, uint32_t source, uint16_t id);
struct packet *waitf(uint8_t port, uint32_t source, uint16_t id, uint16_t frag);

/* dump family */
void           dump (uint8_t port);
void           dumps(uint8_t port, uint32_t source);
void           dumpn(uint8_t port, uint32_t source, uint64_t inode);
void           dumpi(uint8_t port, uint32_t source, uint16_t id);
void           dumpf(uint8_t port, uint32_t source, uint16_t id, uint16_t frag);

/* events */
typedef void (*event_t)(uint32_t, struct packet*);

event_t        when (uint8_t port, event_t handler);

extern event_t event_handler[256];
extern uint32_t m_event_handler;

#endif/*FLUX_IPC_H*/
