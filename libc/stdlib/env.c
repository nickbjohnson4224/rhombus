/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdlib.h>
#include <stdint.h>
#include <dict.h>
#include <string.h>

const char *envroot = "env:";

const char *getenv(const char *name) {
	char *key;
	const char *value;

	key = malloc(strlen(name) + strlen(envroot) + 1);

	strcpy(key, envroot);
	strcat(key, name);

	value = (const char*) dict_readstr(key);

	free(key);

	return value;
}

int setenv(const char *name, const char *value) {
	char *key;

	key = malloc(strlen(name) + strlen(envroot) + 1);
	
	strcpy(key, envroot);
	strcat(key, name);

	dict_writestr(key, (const uint8_t*) value, strlen(value));

	free(key);

	return 0;
}
