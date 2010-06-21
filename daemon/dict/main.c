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

struct dict_node *dict;
uint32_t m_dict;

void dict_handle(uint32_t caller, struct packet *packet) {
	return;
}

int main() {
	dict = calloc(sizeof(struct dict_node), 1);	

	mutex_spin(&m_dict);

	dict_add(dict, "/", "0:0");
	dict_lnk(dict, "//", "/");
	dict_lnk(dict, "/./", "/");
	dict_lnk(dict, "/../", "/");

	printf("%s\n", dict_get(dict, "/"));
	printf("%s\n", dict_get(dict, "/./../..//"));
	printf("%s\n", dict_get(dict, "/.///"));

	mutex_free(&m_dict);

	when(PORT_DICT, dict_handle);

	printf("dict: ready\n");

	send(PORT_SYNC, getppid(), NULL);
	_done();

	return 0;
}
