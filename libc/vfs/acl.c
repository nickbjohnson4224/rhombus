/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <stdlib.h>
#include <vfs.h>

/*****************************************************************************
 * acl_get
 *
 * Returns the permission bitmap for the given user on the given ACL. Returns
 * the value for <user> == -1 if the given user is not in the ACL, and if
 * there is no entry for user -1, zero.
 */

uint8_t acl_get(struct vfs_acl *acl, uint32_t user) {
	struct vfs_acl *ace;

	if (user == 0) {
		return 0xFF;
	}

	for (ace = acl; ace; ace = ace->next) {
		if (ace->user == user) {
			return ace->permit;
		}
	}

	return acl_get_default(acl);
}

/*****************************************************************************
 * acl_set
 *
 * Sets the permission bitmap for the given user on the given ACL. Returns a
 * pointer to the new ACL, which may be a different pointer than <acl>.
 * Returns NULL on out of memory error.
 */

struct vfs_acl *acl_set(struct vfs_acl *acl, uint32_t user, uint8_t permit) {
	struct vfs_acl *ace;

	for (ace = acl; ace; ace = ace->next) {
		if (ace->user == user) {
			ace->permit = permit;
			return acl;
		}
	}

	ace = malloc(sizeof(struct vfs_acl));

	if (ace) {
		ace->user   = user;
		ace->permit = permit;
		ace->next   = acl;
		return ace;
	}
	else {
		return NULL;
	}
}

/*****************************************************************************
 * acl_get_default
 *
 * Returns the permission bitmap for the default user (user -1) on the given
 * ACL. Returns zero if there is no default permission bitmap.
 */

uint8_t acl_get_default(struct vfs_acl *acl) {
	struct vfs_acl *ace;

	for (ace = acl; ace; ace = ace->next) {
		if (ace->user == 0) {
			return ace->permit;
		}
	}

	return 0;
}

/*****************************************************************************
 * acl_set_default
 *
 * Sets the permission bitmap for the default user (user -1) on the given ACL.
 * Returns a pointer to the new ACL, which may be different that <acl> on
 * success, NULL on out of memory error.
 */

struct vfs_acl *acl_set_default(struct vfs_acl *acl, uint8_t permit) {
	return acl_set(acl, 0, permit);
}

/*****************************************************************************
 * acl_free
 *
 * Frees an ACL structure fully.
 */

void acl_free(struct vfs_acl *acl) {

	if (acl) {
		acl_free(acl->next);
		free(acl);
	}
}
