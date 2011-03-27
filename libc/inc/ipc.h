/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and distribute this software for any
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

#ifndef IPC_H
#define IPC_H

#include <stdbool.h>
#include <stdint.h>
#include <natio.h>
#include <arch.h>

/* port numbers ************************************************************/

// kernel events and signals
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

// I/O and similar
#define PORT_REPLY	16
#define PORT_READ	17
#define PORT_WRITE	18
#define PORT_SYNC	19
#define PORT_RESET	20
#define PORT_SHARE	21
#define PORT_RCALL	22
#define PORT_EVENT	23

// filesystem
#define PORT_FIND	32
#define PORT_CONS	33
#define PORT_MOVE	34
#define PORT_REMV	35
#define PORT_LINK	36
#define PORT_LIST	37
#define PORT_SIZE	38
#define PORT_TYPE	39
#define PORT_PERM	40
#define PORT_AUTH	41

/* message structure ********************************************************/

struct msg {
	uint64_t source; // resource pointer of source
	uint64_t target; // resource pointer of target
	uint32_t length; // length of _data_ (not whole message)
	uint8_t  port;   // port (to be) sent to
	uint8_t  arch;   // architecture (i.e. byte order)
	uint16_t padding;
	uint8_t  data[]; // contained data
} __attribute__((packed));

#define ARCH_LEND	0
#define ARCH_BEND	1
#define ARCH_NAT	ARCH_LEND

/* queueing *****************************************************************/

int         mqueue_push(struct msg *msg);
struct msg *mqueue_pull(uint8_t port, uint64_t source);

/* sending and recieving ****************************************************/

int         msend(struct msg *msg);
struct msg *mwait(uint8_t port, uint64_t source);

/* high level send functions ************************************************/

int mreply(struct msg *msg);
int merror(struct msg *msg);
int msendb(uint64_t target, uint8_t port);
int msendv(uint64_t target, uint8_t port, void *data, size_t length);

/* event handlers ***********************************************************/

void when(uint8_t port, void (*handler)(struct msg *msg));

#endif/*IPC_H*/
