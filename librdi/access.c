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

#include <robject.h>
#include <stdlib.h>
#include <mutex.h>
#include <proc.h>

#include <rdi/core.h>

/*
 * AVL tree implementation for access control list
 */

static void _update_height_balance(struct rdi_access_node *root) {
	uint32_t height_l;
	uint32_t height_r;

	height_l = (root->l) ? root->l->height + 1 : 0;
	height_r = (root->r) ? root->r->height + 1 : 0;

	root->height = (height_l > height_r) ? height_l : height_r;
	root->balance = height_l - height_r;
}

static struct rdi_access_node *_balance(struct rdi_access_node *root) {
	struct rdi_access_node *temp;

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

static struct rdi_access_node *_new(uint32_t uid, uint8_t access) {
	struct rdi_access_node *new_node;

	new_node = malloc(sizeof(struct rdi_access_node));
	if (!new_node) return NULL;

	new_node->l = NULL;
	new_node->r = NULL;
	new_node->height = 0;
	new_node->balance = 0;
	new_node->uid = uid;
	new_node->access = access;

	return new_node;
}

static struct rdi_access_node *_get(struct rdi_access_node *root, uint32_t uid) {
	
	while (root) {
		if (root->uid == uid) {
			return root;
		}

		root = (uid < root->uid) ? root->l : root->r;
	}

	return NULL;
}

static struct rdi_access_node *_set(struct rdi_access_node *root, uint32_t uid, uint8_t access) {
	
	if (!root) {
		return _new(uid, access);
	}

	if (uid == root->uid) {
		root->access = access;
		return root;
	}

	if (uid < root->uid) {
		root->l = _set(root->l, uid, access);
	}
	else {
		root->r = _set(root->r, uid, access);
	}

	_update_height_balance(root);
	return _balance(root);
}

static uint32_t _max(struct rdi_access_node *root) {	
	return (root) ? (root->r) ? _max(root->r) : root->uid : 0;
}

static struct rdi_access_node *_del(struct rdi_access_node *root, uint32_t uid) {
	struct rdi_access_node *temp;
	uint32_t max;

	if (!root) {
		return NULL;
	}

	if (root->uid == uid) {
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
		max = _max(root->l);
		temp = _get(root->l, max);
		temp = _new(max, temp->access);
		temp->l = _del(root->l, max);
		temp->r = root->r;
		free(root);
		
		_update_height_balance(temp);
		return _balance(temp);
	}

	if (uid < root->uid) {
		root->l = _del(root->l, uid);
		return root;
	}
	else {
		root->r = _del(root->r, uid);
		return root;
	}
}

/*
 * Interface
 */

uint8_t rdi_get_access(struct robject *ro, uint32_t uid) {
	struct rdi_access_node *node;
	struct rdi_access *acl;
	uint8_t access;

	acl = robject_data(ro, "access");

	if (acl) {
		mutex_spin(&acl->mutex);
		node = _get(acl->root, uid);
		if (node) {
			access = node->access;
		}
		else {
			access = acl->access_default;
		}
		mutex_free(&acl->mutex);
	}
	else {
		access = 0;
	}

	return access;
}

bool rdi_check_access(struct robject *ro, rp_t source, uint8_t access) {

	if (source == 0) {
		return true;
	}

	return ((access & rdi_get_access(ro, getuser(RP_PID(source)))) == access);
}

void rdi_set_access(struct robject *ro, uint32_t uid, uint8_t access) {
	struct rdi_access *acl;

	acl = robject_data(ro, "access");

	if (!acl) {
		acl = malloc(sizeof(struct rdi_access));
		acl->mutex = false;
		acl->root = NULL;
		acl->access_default = 0;
		robject_set_data(ro, "access", acl);
	}

	mutex_spin(&acl->mutex);
	acl->root = _set(acl->root, uid, access);
	mutex_free(&acl->mutex);
}

void rdi_del_access(struct robject *ro, uint32_t uid) {
	struct rdi_access *acl;

	acl = robject_data(ro, "access");
	
	if (!acl) {
		return;
	}

	mutex_spin(&acl->mutex);
	acl->root = _del(acl->root, uid);
	mutex_free(&acl->mutex);
}

void rdi_set_access_default(struct robject *ro, uint8_t access) {
	struct rdi_access *acl;

	acl = robject_data(ro, "access");

	if (!acl) {
		acl = malloc(sizeof(struct rdi_access));
		acl->mutex = false;
		acl->root = NULL;
		acl->access_default = 0;
		robject_set_data(ro, "access", acl);
	}

	mutex_spin(&acl->mutex);
	acl->access_default = access;
	mutex_free(&acl->mutex);
}
