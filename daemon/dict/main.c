/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/ipc.h>
#include <flux/proc.h>
#include <flux/io.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "dict.h"

int main() {
	struct dict_node *d = calloc(sizeof(struct dict_node), 1);	

	dict_add(d, "hello/", "world");

	printf("hello/: %s\n", dict_get(d, "hello/"));

	dict_add(d, "potato/", "potato");

	printf("hello/: %s\n", dict_get(d, "hello/"));
	printf("potato/: %s\n", dict_get(d, "potato/"));

	printf("dict: ready\n");

	send(PORT_SYNC, getppid(), NULL);
	_done();

	return 0;
}
