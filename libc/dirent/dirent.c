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

#include <rhombus.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <rho/natio.h>

#include <dirent.h>

DIR *opendir(const char *dirname) {
	DIR *dirp;
	char *list;

	dirp = malloc(sizeof(DIR));
	if (!dirp) return NULL;

	dirp->fd = open(dirname, ACCS_READ);
	if (dirp->fd < 0) {
		free(dirp);
		return NULL;
	}

	list = rp_list(fd_rp(dirp->fd));

	dirp->entryv = strparse(list, "\t");

	if (!dirp->entryv) {
		free(dirp);
		return NULL;
	}

	dirp->dirpos = 0;

	return dirp;
}

int closedir(DIR *dirp) {
	size_t i;
	
	if (!dirp) {
		return -1;
	}

	if (dirp->entryv) {
		for (i = 0; dirp->entryv[i]; i++) {
			free(dirp->entryv[i]);
		}
		free(dirp->entryv);
	}

	close(dirp->fd);
	free(dirp);

	return 0;
}

struct dirent *readdir(DIR *dirp) {
	static struct dirent dirent;

	if (!dirp) {
		return NULL;
	}

	if (dirp->fd < 0) {
		errno = EBADF;
		return NULL;
	}

	if (!dirp->entryv[dirp->dirpos]) {
		return NULL;
	}

	strlcpy(dirent.d_name, dirp->entryv[dirp->dirpos], NAME_MAX);
	dirent.d_ino = rp_find(fd_rp(dirp->fd), dirp->entryv[dirp->dirpos]);

	dirp->dirpos++;

	return &dirent;
}

int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result) {
	
	if (!dirp) {
		return -1;
	}

	if (dirp->fd < 0) {
		errno = EBADF;
		return -1;
	}

	if (!dirp->entryv[dirp->dirpos]) {
		return -1;
	}

	strlcpy(entry->d_name, dirp->entryv[dirp->dirpos], NAME_MAX);
	entry->d_ino = rp_find(fd_rp(dirp->fd), dirp->entryv[dirp->dirpos]);

	dirp->dirpos++;

	if (result) {
		*result = entry;
	}

	return 0;
}

void rewinddir(DIR *dirp) {
	
	if (dirp) {
		dirp->dirpos = 0;
	}
}

void seekdir(DIR *dirp, long int loc) {
	
	if (dirp) {
		dirp->dirpos = (size_t) loc;
	}
}

long int telldir(DIR *dirp) {
	
	if (dirp) {
		return (long int) dirp->dirpos;
	}

	return -1;
}
