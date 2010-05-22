/*
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/io.h>
#include <flux/ipc.h>
#include <flux/packet.h>

size_t write(int fd, void *buf, size_t size, uint64_t offset) {
	struct packet *p_out;
	struct packet *p_in;
	struct packet *p_err;
	struct file   *file;
	uint8_t *data;
	uint16_t datasize;
	size_t   oldsize, i;
	event_t  old_handler;

	old_handler = when(PORT_REPLY, NULL);
	oldsize     = size;
	p_out       = packet_alloc(0);
	p_in        = NULL;
	file        = fdget(fd);
	data        = buf;
	i           = 0;

	while (size) {
		datasize = (size > PACKET_MAXDATA) ? PACKET_MAXDATA : size;

		packet_setbuf(&p_out, datasize);

		p_out->identity = i;
		p_out->protocol = PACKET_PROTOCOL;
		p_out->software = PACKET_SOFTWARE;
		p_out->encoding = PACKET_ENC_ASCII;
		p_out->flags    = 0;

		p_out->fragment_index = 0;
		p_out->fragment_count = 0;	
		p_out->target_pid     = file->target;
		p_out->target_inode   = file->resource;
		p_out->offset         = offset;

		arch_memcpy(packet_getbuf(p_out), data, datasize);

		send(PORT_WRITE, file->target, p_out);
		p_in = waits(PORT_REPLY, file->target);

		if (p_err = recvs(PORT_ERROR, file->target)) {
			packet_free(p_in);
			packet_free(p_err);

			when(PORT_REPLY, old_handler);
			return (oldsize - size);
		}

		data    = &data[p_in->data_length];
		size   -= p_in->data_length;
		offset += p_in->data_length;
		packet_free(p_in);

		i++;
	}

	packet_free(p_out);
	when(PORT_REPLY, old_handler);

	return oldsize;
}
