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

#include <natio.h>

/****************************************************************************
 * lfs_get_perm
 *
 * Gets the permission mask of a lfs file <node> for the given user <user>.
 * Returns the mask on success, zero on failure.
 */

uint8_t lfs_get_perm(struct lfs_node *node, uint32_t user) {
	
	if (!node) {
		return 0;
	}

	if (user == node->user) {
		return node->perm_user;
	}
	else {
		return node->perm_def;
	}
}

/****************************************************************************
 * lfs_set_perm
 *
 * Sets the permission mask on file <node> for user <user> to <perm>. If
 * user is -1, the operation applies to the default user. Returns 0 on
 * success, nonzero on failure.
 */

int lfs_set_perm(struct lfs_node *node, uint32_t user, uint8_t perm) {
	
	if (!node) {
		return 1;
	}

	if (user == node->user) {
		node->perm_user = perm;
	}
	else if (user == (uint32_t) -1) {
		node->perm_def = perm;
	}
	else {
		return 1;
	}

	return 0;
}
