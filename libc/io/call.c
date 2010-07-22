/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdlib.h>
#include <string.h>
#include <ipc.h>
#include <io.h>

char *call(int fd, const char *name, const char *args) {
	struct call *header;
	size_t size;
	char *ret;

	size = sizeof(struct call) + strlen(args) + 1;

	header = malloc(size);
	strlcpy(header->name, name, 16);
	strcpy (header->args, args);
	header->size = strlen(args);

	psend(fd, (char*) header, (char*) header, size, 0, PORT_CALL);

	if (header->name[0] == '!') {
		ret = NULL;
	}
	else {
		ret = malloc(strlen(header->args) + 1);
		strcpy(ret, header->args);
	}

	free(header);
	return ret;
}
