/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef FLUX_DICT_H
#define FLUX_DICT_H

#include <stdint.h>
#include <stdbool.h>
#include <vdatum.h>
#include <ipc.h>

/* dictionary interface ****************************************************/

struct dict_link {
	const uint8_t *prefix;
	size_t prefixlen;
	uint32_t server;
	uint64_t inode;
};

struct dict_link_req {
	uint8_t key[1024];
	size_t  keylen;
	uint8_t val[1024];
	size_t  vallen;
};

struct dict {
	struct dict *next[256];
	const uint8_t *value;
	size_t vallen;
	struct dict_link *link;
};

extern struct dict_info {
	struct dict root;
	bool        mutex;
	uintptr_t   brk;
} *dict_info;

void dict_init(void);

/* read functions **********************************************************/

const void *dict_read
	(const void *key, size_t keylen, size_t *vallen);

const void *dict_readstr
	(const char *key, size_t *vallen);

const void *dict_readstrns
	(const char *namespace, const char *key, size_t *vallen);

vdatum dict_readvd(vdatum key);

vdatum dict_readvdns(const char *namespace, vdatum key);

/* write functions *********************************************************/

void dict_write
	(const void *key, size_t keylen, 
	const void *val, size_t vallen);

void dict_writestr
	(const char *key, const void *val, size_t vallen);

void dict_writestrns
	(const char *namespace, const char *key,
	const void *val, size_t vallen);

vdatum dict_writevd(vdatum key, vdatum val);

vdatum dict_writevdns(const char *namespace, vdatum key, vdatum val);

/* link functions **********************************************************/

void dict_link
	(const void *key, size_t keylen, 
	const void *prefix, size_t prefixlen,
	uint32_t server, uint64_t inode);

void dict_linkstr
	(const char *key, const char *prefix, uint32_t server, uint64_t inode);

void dict_linkstrns
	(const char *namespace, const char *key,
	const char *prefix, uint32_t server, uint64_t inode);

vdatum dict_linkvd
	(vdatum key, vdatum prefix, uint32_t server, uint64_t inode);

vdatum dict_linkvdns
	(const char *namespace, vdatum key, vdatum prefix,
	uint32_t server, uint64_t inode);

/* internal functions ******************************************************/

void dict_link_read 
	(struct dict_link *link, const void *key, size_t keylen, 
	void *value, size_t *vallen);

void dict_link_write
	(struct dict_link *link, const void *key, size_t keylen,
	const void *value, size_t vallen);

void dict_link_link
	(struct dict_link *link, const void *key, size_t keylen,
	struct dict_link *newlink);

/* dictionary event handlers ***********************************************/

void _dict_read (uint32_t caller, struct packet *packet);
void _dict_write(uint32_t caller, struct packet *packet);
void _dict_link (uint32_t caller, struct packet *packet);

/* dictionary heap - garbage collected *************************************/

void *dict_alloc(size_t size, bool data);
void  dict_sweep(void);

#endif/*FLUX_DICT_H*/
