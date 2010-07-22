/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdlib.h>
#include <ipc.h>

/****************************************************************************
 * packet_setbuf
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

bool packet_setbuf(struct packet **packet, uint32_t length) {

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
 * packet_getbuf
 *
 * Returns a pointer to the data buffer contained within the given packet.
 * The size of this buffer is packet.data_length bytes. Returns NULL on
 * error.
 */

void *packet_getbuf(struct packet *packet) {

	if (!packet) {
		return NULL;
	}

	return (void*) ((uintptr_t) packet + packet->data_offset);
}

/****************************************************************************
 * packet_alloc
 *
 * Returns a pointer to a packet suitable for use with the send() system call.
 * Contents of the packet's memory are undefined.
 */

void *packet_alloc(uint32_t size) {
	return valloc(size);
}

/****************************************************************************
 * packet_free
 *
 * Frees a packet and all of its associated memory.
 */

void packet_free(struct packet *packet) {
	free(packet);
}
