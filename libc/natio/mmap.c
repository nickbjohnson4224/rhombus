/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <rho/natio.h>
#include <rho/proc.h>
#include <rho/page.h>
#include <rho/ipc.h>

/*****************************************************************************
 * rp_mmap
 *
 * Request that pages of memory be shared from another process. Returns a
 * pointer to the shared region of memory on success, NULL on error. If <addr>
 * is provided, the returned pages are remapped to that address.
 *
 * protocol:
 *   port: PORT_MMAP
 *
 *   request:
 *   	uint64_t offset
 *   	uint32_t size
 *   	uint32_t page_flags
 *   
 *   reply:
 *   	... (to align to PAGESZ)
 *   	uint8_t pages[]
 */

void *rp_mmap(rp_t rp, void *addr, size_t size, off_t offset, int prot) {
	struct msg *msg;
	int allocated;

	// allocate buffer if needed
	if (!addr) {
		addr = aalloc(size, PAGESZ);
		
		if (!addr) {
			errno = ENOMEM;
			return NULL;
		}
		
		allocated = 1;
	}
	else {
		allocated = 0;
	}

	// check alignment
	if ((uintptr_t) addr % PAGESZ) {
		errno = EINVAL;

		if (allocated) {
			free(addr);
		}
		
		return NULL;
	}

	// allocate message
	msg = aalloc(sizeof(struct msg) + sizeof(uint64_t) + 2 * sizeof(uint32_t), PAGESZ);

	// check allocation
	if (!msg) {
		errno = ENOMEM;

		if (allocated) {
			free(addr);
		}

		return NULL;
	}

	// format message
	msg->source = RP_CURRENT_THREAD;
	msg->target = rp;
	msg->length = sizeof(uint64_t) + 2 * sizeof(uint32_t);
	msg->port   = PORT_MMAP;
	msg->arch   = ARCH_NAT;
	((uint64_t*) msg->data)[0] = offset;
	((uint32_t*) msg->data)[2] = size;
	((uint32_t*) msg->data)[3] = prot;

	// send message
	if (msend(msg)) {
		errno = EBADMSG;

		if (allocated) {
			free(addr);
		}
		
		return NULL;
	}

	// recieve message
	msg = mwait(PORT_REPLY, rp);

	// check message
	if (msg->length != size + PAGESZ - sizeof(struct msg)) {
		errno = EBADMSG;
		free(msg);

		if (allocated) {
			free(addr);
		}

		return NULL;
	}

	// map recieved pages to <addr>
	page_self(&msg->data[PAGESZ - sizeof(struct msg)], addr, size);
	
	free(msg);
	return addr;
}
