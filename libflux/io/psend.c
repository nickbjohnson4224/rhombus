/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/io.h>
#include <flux/ipc.h>

size_t psend(int fd, char *r, char *s, size_t size, uint64_t off, uint8_t port) {
	struct packet *p_out;
	struct packet *p_in;
	struct file   *file;
	size_t datasize, oldsize, frag;
	size_t spos, rpos;
	event_t old_handler;

	old_handler = when(PORT_REPLY, NULL);
	oldsize     = size;
	p_out       = packet_alloc(0);
	p_in        = NULL;
	file        = fdget(fd);
	frag        = 0;
	spos        = 0;
	rpos        = 0;

	while (size) {
		datasize = (size > PACKET_MAXDATA) ? PACKET_MAXDATA : size;

		packet_setbuf(&p_out, datasize);

		p_out->identity = 0;
		p_out->protocol = PACKET_PROTOCOL;

		p_out->fragment_index = frag;
		p_out->fragment_count = ((size - 1) / PACKET_MAXDATA + 1);
		p_out->target_pid     = file->server;
		p_out->target_inode   = file->inode;
		p_out->offset         = off;

		if (s) {
			arch_memcpy(packet_getbuf(p_out), &s[spos], datasize);
			spos += datasize;
		}

		send(port, file->server, p_out);
		p_in = waits(PORT_REPLY, file->server);

		if (!p_in || p_in->data_length == 0) {
			if (p_in) packet_free(p_in);
			break;
		}

		if (r) {
			arch_memcpy(&r[rpos], packet_getbuf(p_in), p_in->data_length);
			rpos += p_in->data_length;
		}

		size -= p_in->data_length;
		off  += p_in->data_length;

		packet_free(p_in);

		frag++;
	}

	packet_free(p_out);
	when(PORT_REPLY, old_handler);

	return (oldsize - size);
}
