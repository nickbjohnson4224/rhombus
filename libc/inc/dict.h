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
#include <stdio.h>
#include <ipc.h>

/* dictionary interface ****************************************************/

struct __link_req {
	char key[2048];
	char val[1024];
};

struct __dict {
	struct __dict *next[256];
	FILE *link;
	char *value;
};

extern struct __dict *dict_root;
extern bool dict_mutex;

void dict_init(void);

/* read functions **********************************************************/

char *dread  (const char *key);
char *dreadr (FILE *remote, const char *key);

/* write functions *********************************************************/

int dwrite   (const char *value, const char *key);
int dwriter  (FILE *remote, const char *value, const char *key);

/* link functions **********************************************************/

int dlink    (const char *key, FILE *target);

/* dictionary event handlers ***********************************************/

void _devent_read (struct packet *packet, uint8_t port, uint32_t caller);
void _devent_write(struct packet *packet, uint8_t port, uint32_t caller);

#endif/*FLUX_DICT_H*/
