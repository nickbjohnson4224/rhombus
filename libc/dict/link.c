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
#include <natio.h>
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

static 
int dlink_rec(struct __dict *root, struct __link *link, const char *key) {

	if (root->link) {
		return _dlink_link(root->link, link, key);
	}

	if (!key[0]) {
		if (link) {
			root->link = link;
		}
		else {
			root->link = NULL;
		}

		return 0;
	}

	if (!root->next[key[0]]) {
		root->next[key[0]] = dalloc(sizeof(struct __dict));
	}

	return dlink_rec(root->next[key[0]], link, &key[1]);
}

/****************************************************************************
 * dlink
 *
 * Write link to dictionary with the given key/link pair. This function
 * is thread-safe.
 */

int dlink(const char *key, const char *pre, FILE *target) {
	struct __link *link;
	int err;

	link = dalloc(sizeof(struct __link));
	link->pre    = strcpy(dalloc(strlen(pre) + 1), pre);
	link->server = target->server;
	link->inode  = target->inode;

	mutex_spin(&dict_info->mutex);
	err = dlink_rec(&dict_info->root, link, key);
	mutex_free(&dict_info->mutex);

	return err;
}

char *_dlink_read(struct __link *link, const char *key) {
	FILE *file;
	size_t size;
	struct __link_req req;
	char *value;

	file = __fcons(link->server, link->inode, NULL);

	strlcpy(req.key, link->pre, 2048);
	strlcat(req.key, key,       2048);

	size = ssend(file, &req, &req, sizeof(struct __link_req), 0, PORT_DREAD);

	if (!size) {
		fclose(file);
		return NULL;
	}

	value = malloc(strlen(req.val) + 1);
	strcpy(value, req.val);

	fclose(file);
	return value;
}

int _dlink_write(struct __link *link, const char *val, const char *key) {
	FILE *file;
	size_t size;
	struct __link_req req;

	file = __fcons(link->server, link->inode, NULL);

	strlcpy(req.key, link->pre, 2048);
	strlcat(req.key, key,       2048);
	strlcpy(req.val, val,       1024);

	size = ssend(file, &req, &req, sizeof(struct __link_req), 0, PORT_DWRITE);

	if (!size) {
		fclose(file);
		return -1;
	}
	else {
		fclose(file);
		return 0;
	}
}

int _dlink_link(struct __link *link, struct __link *new, const char *key) {
	FILE *file;
	size_t size;
	struct __link_req req;

	file = __fcons(link->server, link->inode, NULL);

	strlcpy(req.key, link->pre, 2048);
	strlcat(req.key, key,       2048);
	strlcpy(req.val, tdeflate(link, sizeof(struct __link_req)), 1024);

	size = ssend(file, &req, &req, sizeof(struct __link_req), 0, PORT_DLINK);

	if (!size) {
		fclose(file);
		return -1;
	}

	fclose(file);

	return 0;
}
