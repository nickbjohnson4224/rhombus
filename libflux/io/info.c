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
	struct info_query query;
	size_t i;

	for (i = 0; field[i]; i++) {
		query.field[i] = field[i];
	}
	query.field[i] = '\0';

	psend(fd, (char*) &query, (char*) &query, sizeof(query), 0, PORT_INFO);

	for (i = 0; query.value[i]; i++) {
		value[i] = query.value[i];
	}
	value[i] = '\0';

	return (value[0] != '\0');
}
