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
 * fs_find
 *
 * Finds the filesystem object with the given path <path> from <root> if it 
 * exists. If it does not exist, this function returns 0.
 */

uint64_t fs_find(uint64_t root, const char *path) {
	struct mp_fs *command;
	uint64_t ret;
	char *path_s;

	path_s = path_simplify(path);

	command = malloc(sizeof(struct mp_fs));
	command->op = FS_FIND;
	command->v0 = 0;
	command->v1 = 0;
	strlcpy(command->s0, path_s, 4000);

	free(path_s);

	command = fs_send(root, command);
	if (!command) {
		errno = EBADMSG;
		return 0;
	}

	/* check for errors */
	if (command->op == FS_ERR) {
		switch (command->v0) {
		case ERR_NULL: errno = EUNK; break;
		case ERR_FILE: errno = ENOENT; break;
		case ERR_DENY: errno = EACCES; break;
		case ERR_FUNC: errno = ENOSYS; break;
		case ERR_TYPE: errno = EUNK; break;
		case ERR_FULL: errno = EUNK; break;
		}

		free(command);
		return 0;
	}

	ret = command->v0;
	free(command);
	return ret;
}
