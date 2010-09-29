/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef FLUX_IPC_H
#define FLUX_IPC_H

#include <stdint.h>
#include <stdbool.h>
#include <arch.h>

/* Flux packet protocol (FPP) **********************************************/

#define PACKET_MAXDATA		(PAGESZ - 40)
#define PACKET_PROTOCOL		4	/* protocol version */

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
	uint32_t source_inode;

	/* target routing information */
	uint32_t target_pid;
	uint32_t target_inode;

	/* target file offset */
	uint64_t offset;

} __attribute__ ((packed));

bool  psetbuf(struct packet **packet, uint32_t length);
void *pgetbuf(struct packet *packet);

void *palloc(uint32_t size);
void  pfree (struct packet *packet);

/* port numbers ************************************************************/

#define PORT_QUIT	0
#define PORT_TERM	1
#define PORT_ABORT	2
#define PORT_KILL	3
#define PORT_STOP	4
#define PORT_CONT	5
#define PORT_TRAP	6
#define PORT_INT	7
#define PORT_IRQ	8
#define PORT_ALARM	9
#define PORT_CHILD	10
#define PORT_FLOAT	11
#define PORT_PAGE	12
#define PORT_ILL	13
#define PORT_USER1	14
#define PORT_USER2	15

#define PORT_READ	16
#define PORT_WRITE	17

#define PORT_DREAD	24
#define PORT_DWRITE	25
#define PORT_DLINK	26

#define PORT_VFS	27

#define PORT_REPLY	32
#define PORT_PING	33

/* queueing ****************************************************************/

struct message {
	struct message *next;
	struct message *prev;
	struct packet *packet;
	uint32_t source;
};

extern struct message msg_queue[256];
extern bool         m_msg_queue[256];

void           pstash(struct packet *packet, uint8_t port, uint32_t source);

/* psend *******************************************************************/

int            psend (uint8_t port, uint32_t target, struct packet *packet);

/* precv family - asynchronous *********************************************/

struct packet *precv (uint8_t port);
struct packet *precvs(uint8_t port, uint32_t source);
struct packet *precvn(uint8_t port, uint32_t source, uint32_t inode);
struct packet *precvi(uint8_t port, uint32_t source, uint16_t id);
struct packet *precvf(uint8_t port, uint32_t source, uint16_t id, uint16_t frag);

/* pwait family - synchronous **********************************************/

struct packet *pwait (uint8_t port);
struct packet *pwaits(uint8_t port, uint32_t source);
struct packet *pwaitn(uint8_t port, uint32_t source, uint32_t inode);
struct packet *pwaiti(uint8_t port, uint32_t source, uint16_t id);
struct packet *pwaitf(uint8_t port, uint32_t source, uint16_t id, uint16_t frag);

/* pdump family ************************************************************/

void           pdump (uint8_t port);
void           pdumps(uint8_t port, uint32_t source);
void           pdumpn(uint8_t port, uint32_t source, uint32_t inode);
void           pdumpi(uint8_t port, uint32_t source, uint16_t id);
void           pdumpf(uint8_t port, uint32_t source, uint16_t id, uint16_t frag);

/* events ******************************************************************/

typedef void (*event_t)(struct packet *packet, uint8_t port, uint32_t source);

event_t        when (uint8_t port, event_t handler);

extern event_t event_handler[256];
extern bool  m_event_handler;

#endif/*FLUX_IPC_H*/
