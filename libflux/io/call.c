/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/ipc.h>
#include <flux/io.h>
#include <flux/heap.h>

char *call(int fd, const char *name, const char *args) {
	struct call *header;
	size_t size;
	char *ret;

	size = sizeof(struct call) + arch_strlen(args) + 1;

	header = heap_malloc(size);
	arch_strlcpy(header->name, name, 16);
	arch_strcpy (header->args, args);
	header->size = arch_strlen(args);

	psend(fd, (char*) header, (char*) header, size, 0, PORT_CALL);

	if (header->name[0] == '!') {
		ret = NULL;
	}
	else {
		ret = heap_malloc(arch_strlen(header->args) + 1);
		arch_strcpy(ret, header->args);
	}

	heap_free(header);
	return ret;
}
