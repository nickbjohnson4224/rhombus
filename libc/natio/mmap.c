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

#include <stdint.h>
#include <stdlib.h>
#include <natio.h>
#include <arch.h>
#include <ipc.h>

/*****************************************************************************
 * mmap
 *
 * Requests a memory mapping from file <fd> at offset <offset> of length
 * <length> with page flags <prot>. Returns a pointer to that mapped region
 * on success, NULL on error.
 *
 * Note that drivers rarely have the ability to memory map - this function is
 * more for shared memory based drivers, like the shared object daemon.
 */

void *mmap(uint64_t fd, size_t length, int prot, uint64_t offset) {
	struct mmap_cmd *cmd;
	struct msg *msg;
	void *region;
	event_t old_handler;
	
	/* save old PORT_REPLY handler */
	old_handler = when(PORT_REPLY, NULL);

	/* allocate message */
	msg = malloc(sizeof(struct msg));
	msg->count = 1;
	msg->packet = aalloc(PAGESZ, PAGESZ);

	/* set up mmap command header */
	cmd = msg->packet;
	cmd->inode  = fd & 0xFFFFFFFF;
	cmd->length = length;
	cmd->offset = offset;
	cmd->prot   = prot;

	/* send message */
	msend(PORT_MMAP, fd >> 32, msg);

	/* recieve response */
	msg = mwaits(PORT_REPLY, fd >> 32);
	region = msg->packet;

	/* free recieved message */
	free(msg);
	
	/* restore old PORT_REPLY handler */
	when(PORT_REPLY, old_handler);

	return region;
}
