/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and distribute this software for any
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

#include <stdint.h>
#include <stdlib.h>

#include <rho/mutex.h>

#include <rdi/robject.h>

static bool _mutex = false;

/*
 * AVL tree implementation for hash table buckets.
 */

struct __bucket {
	struct __bucket *l;
	struct __bucket *r;
	int32_t balance;
	uint32_t height;

	uint32_t index;
	struct robject *ro;
};

static void _update_height_balance(struct __bucket *root) {
	uint32_t height_l;
	uint32_t height_r;

	height_l = (root->l) ? root->l->height + 1 : 0;
	height_r = (root->r) ? root->r->height + 1 : 0;
	root->height = (height_l > height_r) ? height_l : height_r;
	root->balance = height_l - height_r;
}

static struct __bucket *_tree_balance(struct __bucket *root) {
	struct __bucket *temp;
	
	if (root->balance < 2 && root->balance > -2) {
		// no balancing needed
		return root;
	}

	if (root->balance < 0) {
		// left rotation needed

		if (root->r->balance > 0) {
			// right rotation on right subtree needed first
			temp = root->r->l;
			root->r->l = temp->r;
			temp->r = root->r;
			root->r = temp;

			_update_height_balance(temp);
			_update_height_balance(root->r);
		}

		temp = root->r;
		root->r = temp->l;
		temp->l = root;

		_update_height_balance(root);
		_update_height_balance(temp);
		return temp;
	}
	else {
		// right rotation needed
		
		if (root->l->balance < 0) {
			// left rotation on left subtree needed first
			temp = root->l->r;
			root->l->r = temp->l;
			temp->l = root->l;
			root->l = temp;

			_update_height_balance(temp);
			_update_height_balance(root->l);
		}

		temp = root->l;
		root->l = temp->r;
		temp->r = root;

		_update_height_balance(root);
		_update_height_balance(temp);
		return temp;
	}
}

static struct __bucket *_new_bucket(struct robject *ro, uint32_t index) {
	struct __bucket *new_bucket;

	new_bucket = malloc(sizeof(struct __bucket));
	if (!new_bucket) return NULL;

	new_bucket->l = NULL;
	new_bucket->r = NULL;
	new_bucket->index = index;
	new_bucket->ro = ro;
	new_bucket->balance = 0;
	new_bucket->height = 0;

	return new_bucket;
}

static struct __bucket *_tree_get(struct __bucket *root, uint32_t index) {
	
	// made this iterative for speed (we want lookup to be as fast as possible)
	while (root) {
		if (index == root->index) {
			return root;
		}

		root = (index < root->index) ? root->l : root->r;
	}

	return NULL;
}

static struct __bucket *_tree_set(struct __bucket *root, uint32_t index, struct robject *ro) {
	
	if (!root) {
		return _new_bucket(ro, index);
	}

	if (index == root->index) {
		root->ro = ro;
		return root;
	}

	if (index < root->index) {
		root->l = _tree_set(root->l, index, ro);
	}
	else {
		root->r = _tree_set(root->r, index, ro);
	}

	_update_height_balance(root);
	return _tree_balance(root);
}

static uint32_t _tree_max_index(struct __bucket *root) {
	
	if (!root) {
		return 0;
	}

	if (root->r) {
		return _tree_max_index(root->r);
	}
	else {
		return root->index;
	}
}

static struct __bucket *_tree_del(struct __bucket *root, uint32_t index) {
	struct __bucket *temp;
	uint32_t max;
	
	if (!root) {
		return NULL;
	}

	if (root->index == index) {
		// found node to be deleted

		if (!root->l && !root->r) {
			// no children
			free(root);
			return NULL;
		}

		if (!root->l) {
			// right child only
			temp = root->r;
			free(root);
			return temp;
		}
		
		if (!root->r) {
			// left child only
			temp = root->l;
			free(root);
			return temp;
		}

		// both children exist
		max = _tree_max_index(root->l);
		temp = _tree_get(root->l, max);
		temp = _new_bucket(temp->ro, max);
		temp->l = _tree_del(root->l, max);
		temp->r = root->r;
		free(root);
		
		_update_height_balance(temp);
		return _tree_balance(temp);
	}

	if (index < root->index) {
		root->l = _tree_del(root->l, index);
		return root;
	}
	else {
		root->r = _tree_del(root->r, index);
		return root;
	}
}

/*
 * Hashtable with trees for chains for object lookup.
 */

static struct __bucket *_table[256];

static uint8_t _hash(uint32_t index) {
	uint8_t hash;

	hash = index >> 24;
	hash = (hash << 5) - hash + (index >> 16);
	hash = (hash << 5) - hash + (index >> 8);
	hash = (hash << 5) - hash + index;

	return hash;
}

static void _set(uint32_t index, struct robject *ro) {
	uint8_t hash = _hash(index);

	_table[hash] = _tree_set(_table[hash], index, ro);
}

static struct robject *_get(uint32_t index) {
	uint8_t hash = _hash(index);
	struct __bucket *bucket;

	bucket = _tree_get(_table[hash], index);

	if (bucket) {
		return bucket->ro;
	}
	else {
		return NULL;
	}
}

static void _del(uint32_t index) {
	uint8_t hash = _hash(index);

	_table[hash] = _tree_del(_table[hash], index);
}

/*
 * Real lookup interface.
 */

void robject_set(uint32_t index, struct robject *ro) {

	mutex_spin(&_mutex);
	if (ro) {
		mutex_spin(&ro->mutex);
		_set(index, ro);
		mutex_free(&ro->mutex);
	}
	else {
		_del(index);
	}
	mutex_free(&_mutex);
}

struct robject *robject_get(uint32_t index) {
	struct robject *robject;

	mutex_spin(&_mutex);
	robject = _get(index);
	mutex_free(&_mutex);

	return robject;
}

/*
 * Real index allocator interface
 */

uint32_t robject_new_index(void) {
	static uint32_t base = 1;
	static bool mutex = false;

	uint32_t index;

	mutex_spin(&mutex);
	index = base;
	base++;
	mutex_free(&mutex);

	return index;
}
