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
#include <stdlib.h>
#include <fs.h>

/*****************************************************************************
 * fs_find
 *
 * Finds the filesystem object with the given path <path> from <root> if it 
 * exists. If it does not exist, this function returns NULL.
 */

FILE *fs_find(FILE *root, const char *path) {
	struct fs_cmd command;
	FILE *file;

	command.op = FS_FIND;
	command.v0 = 0;
	command.v1 = 0;
	strlcpy(command.s0, path, 4000);
	
	if (!fs_send(root, &command)) {
		return NULL;
	}

	file = malloc(sizeof(FILE));
	file->server = ((command.v0 >> 32) & 0xFFFFFFFF);
	file->inode  = (command.v0 & 0xFFFFFFFF);
	file->ext    = NULL;

	return file;
}
