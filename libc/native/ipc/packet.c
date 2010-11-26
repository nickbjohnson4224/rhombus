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

#include <stdlib.h>
#include <ipc.h>

/****************************************************************************
 * psetbuf
 *
 * Sets the buffer size of a packet structure. If successful, this function
 * returns true and the size of the buffer returned by packet_getbuf will be
 * of size length; otherwise, this function returns false. 
 *
 * The size of the packet in memory will be adjusted to accomodate the 
 * buffer, and the pointer may be modified to point to the new packet memory. 
 * The maximum value of length is PACKET_MAXDATA. 
 *
 * The data contained in this buffer after calling this function is 
 * completely undefined; the header information is unchanged except for 
 * packet.data_length and packet.data_offset.
 */

bool psetbuf(struct packet **packet, uint32_t length) {

	if (!*packet) {
		return false;
	}

	if (length > PACKET_MAXDATA) {
		return false;
	}

	(*packet)->data_offset = sizeof(struct packet);
	(*packet)->data_length = length;

	return true;
}

/****************************************************************************
 * pgetbuf
 *
 * Returns a pointer to the data buffer contained within the given packet.
 * The size of this buffer is packet.data_length bytes. Returns NULL on
 * error.
 */

void *pgetbuf(struct packet *packet) {

	if (!packet) {
		return NULL;
	}

	return (void*) ((uintptr_t) packet + packet->data_offset);
}

/****************************************************************************
 * palloc
 *
 * Returns a pointer to a packet suitable for use with the psend() system 
 * call. Contents of the packet's memory are undefined.
 */

void *palloc(uint32_t size) {
	return valloc(size);
}

/****************************************************************************
 * pfree
 *
 * Frees a packet and all of its associated memory.
 */

void pfree(struct packet *packet) {
	free(packet);
}
