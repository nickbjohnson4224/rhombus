/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/io.h>
#include <flux/ipc.h>
#include <flux/packet.h>
#include <flux/proc.h>

bool info(int fd, char *value, const char *field) {
	struct packet *p_out;
	struct packet *p_in;
	struct info_query *query;
	struct file   *file;
	size_t i;
	event_t old_handler;

	old_handler = when(PORT_REPLY, NULL);
	p_out       = packet_alloc(0);
	file        = fdget(fd);

	packet_setbuf(&p_out, sizeof(struct info_query));
	query = packet_getbuf(p_out);

	p_out->identity = 0;
	p_out->protocol = PACKET_PROTOCOL;
	p_out->software = PACKET_SOFTWARE;
	p_out->encoding = PACKET_ENC_ASCII;
	p_out->flags    = 0;

	p_out->fragment_index = 0;
	p_out->fragment_count = 0;

	p_out->source_pid     = getpid();
	p_out->source_inode   = 0;
	p_out->target_pid     = file->target;
	p_out->target_inode   = file->resource;

	for (i = 0; field[i]; i++) {
		query->field[i] = field[i];
	}
	query->field[i] = '\0';

	send(PORT_INFO, file->target, p_out);
	p_in = waits(PORT_REPLY, file->target);

	query = packet_getbuf(p_in);

	for (i = 0; query->value[i]; i++) {
		value[i] = query->value[i];
	}
	value[i] = '\0';

	packet_free(p_in);
	packet_free(p_out);

	when(PORT_REPLY, old_handler);

	return (value[0] != '\0');
}
