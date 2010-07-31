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

#include <dict.h>
#include <string.h>
#include <stdio.h>
#include <ipc.h>
#include <mutex.h>
#include <stdlib.h>

/****************************************************************************
 * dict_link_rec
 *
 * Recursive dictionary linking function, used by interface link functions.
 * Sets the dictionary link to the given value unless the given value is
 * null, in which case the link is deleted. This function is not thread-safe.
 */

static void dict_link_rec
		(struct dict *root, const uint8_t *key, size_t keylen, size_t keypos,
		struct dict_link *link) {

	if (root->link) {
		dict_link_link(root->link, &key[keypos], keylen - keypos, link);

		return;
	}

	if (keypos >= keylen) {
		if (link) {
			root->link = memcpy(dict_alloc(sizeof(struct dict_link), true), 
				link, sizeof(struct dict_link));
		}
		else {
			root->link = NULL;
		}

		return;
	}

	if (!root->next[key[keypos]]) {
		root->next[key[keypos]] = dict_alloc(sizeof(struct dict), false);
	}

	dict_link_rec(root->next[key[keypos]], key, keylen, keypos + 1, link);
}

/****************************************************************************
 * dict_link
 *
 * Write link to dictionary with the given key/link pair. This function
 * is thread-safe.
 */

void dict_link
		(const uint8_t *key, size_t keylen, const uint8_t *prefix, 
		size_t prefixlen, uint32_t server, uint64_t inode) {

	struct dict_link *link;

	link = malloc(sizeof(struct dict_link));
	link->prefix = prefix;
	link->prefixlen = prefixlen;
	link->server = server;
	link->inode = inode;

	mutex_spin(&dict_info->mutex);

	dict_link_rec(&dict_info->root, key, keylen, 0, link);

	mutex_free(&dict_info->mutex);

	free(link);
}

/****************************************************************************
 * dict_linkstr
 */

void dict_linkstr
		(const char *key, const char *prefix, 
		uint32_t server, uint64_t inode) {

	dict_link((uint8_t*) key, strlen(key), (uint8_t*) prefix, strlen(prefix),
		server, inode);
}

/****************************************************************************
 * dict_linkstrns
 */

void dict_linkstrns
		(const char *namespace, const char *key, const char *prefix,
		uint32_t server, uint64_t inode) {
	
	char *buffer;

	buffer = malloc(strlen(namespace) + strlen(key) + 1);
	strcpy(buffer, namespace);
	strcat(buffer, key);

	dict_linkstr(buffer, prefix, server, inode);

	free(buffer);
}

void dict_link_read
		(struct dict_link *link, const uint8_t *key, size_t keylen,
		uint8_t *value, size_t *vallen) {

	FILE *file;
	size_t size;
	struct dict_link_req request;

	file = fcons(link->server, link->inode);

	memcpy(&request.key[0], link->prefix, link->prefixlen);
	memcpy(&request.key[link->prefixlen], key, keylen);
	request.keylen = link->prefixlen + keylen;

	size = ssend(file, &request, &request, 
		sizeof(struct dict_link_req), 0, PORT_DREAD);

	if (!size) {
		*vallen = 0;
		fclose(file);
		return;
	}

	if (*vallen < request.vallen) {
		memcpy(value, request.val, *vallen);
	}
	else {
		memcpy(value, request.val, request.vallen);
		*vallen = request.vallen;
	}

	fclose(file);
}

void dict_link_write
		(struct dict_link *link, const uint8_t *key, size_t keylen,
		const uint8_t *value, size_t vallen) {
	
	FILE *file;
	struct dict_link_req request;

	file = fcons(link->server, link->inode);

	memcpy(&request.key[0], link->prefix, link->prefixlen);
	memcpy(&request.key[link->prefixlen], key, keylen);
	request.keylen = link->prefixlen + keylen;

	if (vallen > 1024) {
		vallen = 1024;
	}

	memcpy(request.val, value, vallen);
	request.vallen = vallen;

	ssend(file, &request, &request, sizeof(struct dict_link_req), 0, PORT_DWRITE);

	fclose(file);
}

void dict_link_link
		(struct dict_link *link, const uint8_t *key, size_t keylen,
		struct dict_link *newlink) {
	
	FILE *file;
	struct dict_link_req request;

	file = fcons(link->server, link->inode);

	memcpy(&request.key[0], link->prefix, link->prefixlen);
	memcpy(&request.key[link->prefixlen], key, keylen);
	request.keylen = link->prefixlen + keylen;

	memcpy(request.val, newlink, sizeof(struct dict_link_req));
	request.vallen = sizeof(struct dict_link_req);

	ssend(file, &request, &request, sizeof(struct dict_link_req), 0, PORT_DLINK);

	fclose(file);
}
