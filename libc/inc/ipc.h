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

#define PORT_REPLY	16
#define PORT_READ	17
#define PORT_WRITE	18
#define PORT_SYNC	19
#define PORT_RESET	20
#define PORT_FS		21
#define PORT_MMAP	22
#define PORT_EVENT	23
#define PORT_CMD	24

void _on_event(void);

/* queueing ****************************************************************/

struct msg {
	uint32_t port;
	uint32_t source;

	uint32_t count;
	void    *packet;

	struct msg *next;
	struct msg *prev;
};

extern struct msg msg_queue[256];
extern bool     m_msg_queue[256];

void mstash(struct msg *msg);

/* msend *******************************************************************/

int msend (uint8_t port, uint32_t target, struct msg *msg);

/* mrecv family - asynchronous *********************************************/

struct msg *mrecv (uint8_t port);
struct msg *mrecvs(uint8_t port, uint32_t source);

/* mwait family - synchronous **********************************************/

struct msg *mwait (uint8_t port);
struct msg *mwaits(uint8_t port, uint32_t source);

/* mdump family ************************************************************/

void mdump (uint8_t port);
void mdumps(uint8_t port, uint32_t source);

/* events ******************************************************************/

typedef void (*event_t)(struct msg *msg);

event_t when(uint8_t port, event_t handler);

extern event_t event_handler[256];
extern bool  m_event_handler;

#endif/*FLUX_IPC_H*/
