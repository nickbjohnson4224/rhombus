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

#include <stdlib.h>
#include <natio.h>
#include <mutex.h>

#define HASHSIZE	256

static struct lfs_node *_node_hash[HASHSIZE];
static bool _m_node_hash[HASHSIZE];

/****************************************************************************
 * lfs_add_node
 *
 * Inserts the given LFS node into a table based on its inode number, where
 * it can be retrieved by that inode number. Returns 0 on success, nonzero on
 * error.
 */

uint32_t lfs_add_node(struct lfs_node *node) {
	size_t index;

	node->next = NULL;
	node->prev = NULL;

	index = node->inode % HASHSIZE;

	mutex_spin(&_m_node_hash[index]);

	if (_node_hash[index]) {
		_node_hash[index]->prev = node;
		node->next = _node_hash[index];
	}
	_node_hash[index] = node;

	mutex_free(&_m_node_hash[index]);

	return 0;
}

/****************************************************************************
 * lfs_get_node
 *
 * Retrieves an LFS node from the table based on its inode number. Returns a
 * pointer to the found LFS node on success, NULL on failure.
 */

struct lfs_node *lfs_get_node(uint32_t inode) {
	struct lfs_node *node;
	size_t index;

	index = inode % HASHSIZE;

	mutex_spin(&_m_node_hash[index]);

	node = _node_hash[index];
	while (node) {
		if (node->inode == inode) {
			break;
		}
		else node = node->next;
	}

	mutex_spin(&_m_node_hash[index]);

	return node;
}

/****************************************************************************
 * lfs_del_node
 *
 * Removes an LFS node from the table based on its inode number. Returns a
 * pointer to the removed node on success, NULL on error.
 *
 * Note - this does not free the LFS node being removed.
 */

struct lfs_node *lfs_del_node(uint32_t inode) {
	struct lfs_node *node;
	size_t index;

	node = lfs_get_node(inode);

	if (node) {
		index = inode % HASHSIZE;

		mutex_spin(&_m_node_hash[index]);
		
		if (node->next) {
			node->next->prev = node->prev;
		}
		if (node->prev) {
			node->prev->next = node->next;
		}

		node->next = NULL;
		node->prev = NULL;

		mutex_free(&_m_node_hash[index]);

		return node;
	}
	else {
		return NULL;
	}
}
