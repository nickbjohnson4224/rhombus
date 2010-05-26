/*
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/io.h>
#include <flux/ipc.h>
#include <flux/packet.h>
#include <flux/proc.h>

#include <stdio.h>

size_t query(int fd, void *rbuf, void *sbuf, size_t size) {
	struct packet *p_in;
	struct packet *p_out;
	struct packet *p_err;
	struct file *file;
	uint8_t *send_data;
	uint8_t *recv_data;
	event_t old_handler;
	size_t oldsize;
	size_t i;

	old_handler = when(PORT_REPLY, NULL);
	p_out       = packet_alloc(0);
	p_in        = NULL;
	oldsize     = size;
	send_data   = sbuf;
	recv_data   = rbuf;
	file        = fdget(fd);
	i           = 0;
	
	packet_setbuf(&p_out, size);

	p_out->identity = 0;
	p_out->protocol = PACKET_PROTOCOL;
	p_out->software = PACKET_SOFTWARE;
	p_out->encoding = PACKET_ENC_UNK;
	p_out->flags    = 0;

	p_out->fragment_index = 0;
	p_out->fragment_count = 0;

	p_out->source_pid     = getpid();
	p_out->source_inode   = 0;
	p_out->target_pid     = file->target;
	p_out->target_inode   = file->resource;

	arch_memcpy(packet_getbuf(p_out), send_data, size);

	send(PORT_QUERY, file->target, p_out);
	p_in = waits(PORT_REPLY, file->target);

	if (p_err = recvs(PORT_ERROR, file->target)) {
		packet_free(p_in);
		packet_free(p_err);

		when(PORT_REPLY, old_handler);
		return 0;
	}

	size = p_in->data_length;
	arch_memcpy(recv_data, packet_getbuf(p_in), size);

	packet_free(p_in);
	packet_free(p_out);

	when(PORT_REPLY, old_handler);

	return size;
}
