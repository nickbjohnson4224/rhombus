/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <ipc.h>

bool info(FILE *fd, char *value, const char *field) {
	struct info_query query;
	size_t i;

	strcpy(query.field, field);
	ssend(fd, (char*) &query, (char*) &query, sizeof(query), 0, PORT_INFO);
	strcpy(value, query.value);

	return (value[0] != '\0');
}
