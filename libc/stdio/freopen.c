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
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <natio.h>

#define MODE_R	0x00
#define MODE_W	0x01
#define MODE_A	0x02
#define MODE_P	0x04
#define MODE_RP	0x04
#define MODE_WP	0x05
#define MODE_AP	0x06

/****************************************************************************
 * freopen
 *
 * Same as fopen(), but result is stored in <stream>.
 */

FILE *freopen(const char *path, const char *mode, FILE *stream) {
	FILE *file;
	int modenum;
	uint8_t perm;

	if (!mode) {
		return NULL;
	}
	else {
		modenum = 0;
		if (mode[0] == 'r') modenum = MODE_R;
		if (mode[0] == 'w') modenum = MODE_W;
		if (mode[0] == 'a') modenum = MODE_A;
		if (mode[1] == '+') modenum |= MODE_P;
	}

	file = vfs_get_file(NULL, path);
	perm = vfs_get_perm(NULL, path, 0);

	if (!file) {
		if (modenum != MODE_R) {
			file = vfs_new_file(NULL, path);
			perm = vfs_get_perm(NULL, path, 0);

			if (!file) {
				return NULL;
			}
		}
		else {
			errno = ENOFILE;
			return NULL;
		}
	}
	
	if ((modenum == MODE_R || modenum == MODE_RP) && !(perm & PERM_READ)) {
		errno = EPERM;
		return NULL;
	}
	if (modenum != MODE_R && !(perm & PERM_WRITE)) {
		errno = ENOSYS;
		return NULL;
	}
	
	__fsetup(file);

	file->ext->size = vfs_get_size(file, "");

	if (!stream) {
		return NULL;
	}
	else {
		stream = calloc(sizeof(FILE), 1);

		if (!stream) {
			errno = ENOMEM;
			return NULL;
		}
	}

	__fstrip(stream);
	memcpy(stream, file, sizeof(FILE));
	free(file);

	return stream;
}
