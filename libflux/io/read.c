/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details 
 */

#include <flux/arch.h>
#include <flux/io.h>
#include <flux/ipc.h>
#include <flux/packet.h>

size_t read(int fd, void *buf, size_t size, uint64_t offset) {
	struct packet *p_out;
	struct packet *p_in;
	struct file   *file;
	uint8_t *data;
	uint16_t datasize;
	size_t   oldsize, i;
	event_t  old_handler;

	old_handler = event(PORT_REPLY, NULL);
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
		p_out->type     = PACKET_TYPE_READ;
		p_out->flags    = 0;

		p_out->fragment_index = 0;
		p_out->fragment_count = 0;	
		p_out->target_pid     = file->target;
		p_out->target_inode   = file->resource;
		p_out->offset         = offset;

		send(PORT_READ, file->target, p_out);

		p_in = waits(PORT_REPLY, file->target);

		if (p_in->type == PACKET_TYPE_ERROR) {
			packet_free(p_in);

			event(PORT_REPLY, old_handler);
			return (oldsize - size);
		}

		arch_memcpy(data, packet_getbuf(p_in), p_in->data_length);

		data    = &data[p_in->data_length];
		size   -= p_in->data_length;
		offset += p_in->data_length;
		packet_free(p_in);

		i++;
	}

	packet_free(p_out);
	event(PORT_REPLY, old_handler);

	return oldsize;
}
