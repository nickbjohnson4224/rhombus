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

#include <stdint.h>
#include <string.h>
#include <natio.h>
#include <errno.h>

/*****************************************************************************
 * fs_list
 *
 * Gives the name of the entry of number <entry> in the directory <dir>.
 * Returns a copy of that string on success, NULL on failure.
 */

char *fs_list(uint64_t dir, int entry) {
	struct fs_cmd command;

	command.op = FS_LIST;
	command.v0 = entry;
	command.v1 = 0;
	
	if (!fs_send(dir, &command)) {
		errno = EBADMSG;
		return NULL;
	}

	/* check for errors */
	if (command.op == FS_ERR) {
		switch (command.v0) {
		case ERR_NULL: errno = 0; break;
		case ERR_FILE: errno = ENOENT; break;
		case ERR_DENY: errno = EACCES; break;
		case ERR_FUNC: errno = ENOSYS; break;
		case ERR_TYPE: errno = ENOTDIR; break;
		case ERR_FULL: errno = EUNK; break;
		}

		return NULL;
	}
	
	command.null0 = '\0';
	return strdup(command.s0);
}
