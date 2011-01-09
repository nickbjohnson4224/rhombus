/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <natio.h>
#include <errno.h>

/*****************************************************************************
 * fs_auth
 *
 * Sets the permissions of the filesystem object <fobj> to <perm> for user
 * <user>. Returns zero on success, nonzero on error.
 */

int fs_auth(uint64_t fobj, uint32_t user, uint8_t perm) {
	struct mp_fs *command;

	command = malloc(sizeof(struct mp_fs));
	command->op = FS_AUTH;
	command->v0 = user;
	command->v1 = perm;
	
	command = fs_send(fobj, command);
	if (!command) {
		errno = EBADMSG;
		return 1;
	}

	/* check for errors */
	if (command->op == FS_ERR) {
		switch (command->v0) {
		case ERR_NULL: errno = EUNK; break;
		case ERR_FILE: errno = ENOENT; break;
		case ERR_DENY: errno = EACCES; break;
		case ERR_FUNC: errno = ENOSYS; break;
		case ERR_TYPE: errno = EUNK; break;
		case ERR_FULL: errno = ENOSPC; break;
		}

		free(command);
		return 1;
	}
	
	free(command);
	return 0;
}
