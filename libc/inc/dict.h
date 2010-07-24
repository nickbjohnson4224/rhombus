/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#ifndef FLUX_DICT_H
#define FLUX_DICT_H

#include <stdint.h>
#include <stdbool.h>

/* dictionary interface ****************************************************/

struct dict {
	struct dict *next[256];
	const uint8_t *value;
};

extern struct dict_info {
	struct dict root;
	bool        mutex;
	uintptr_t   brk;
} *dict_info;

void dict_init(void);

const uint8_t *dict_read
	(const uint8_t *key, size_t keylen);

const uint8_t *dict_readstr
	(const char *key);

const uint8_t *dict_readstrns
	(const char *namespace, const char *key);

void dict_write
	(const uint8_t *key, size_t keylen, 
	const uint8_t *val, size_t vallen);

void dict_writestr
	(const char *key, const uint8_t *val, size_t vallen);

void dict_writestrns
	(const char *namespace, const char *key, 
	const uint8_t *val, size_t vallen);

void dict_setlink
	(const uint8_t *key, size_t keylen, 
	const uint8_t *prefix, size_t prefixlen,
	uint32_t server, uint64_t inode);

/* dictionary heap - garbage collected *************************************/

void *dict_alloc(size_t size, bool data);
void  dict_sweep(void);

#endif/*FLUX_DICT_H*/
