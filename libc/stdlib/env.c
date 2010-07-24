/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdlib.h>
#include <stdint.h>
#include <dict.h>
#include <string.h>

const char *getenv(const char *name) {

	return (const char*) dict_readstrns("env:", name);
}

int setenv(const char *name, const char *value) {

	dict_writestrns("env:", name, (const uint8_t*) value, strlen(value));

	return 0;
}
