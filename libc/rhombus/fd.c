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
#include <stdlib.h>
#include <mutex.h>
	
/*****************************************************************************
 * File Descriptor Table
 */

struct fdtab {
	int mode;
	rp_t rp;
};

static struct fdtab *_fdtab;
static bool _fdtab_mutex;
static int _fdtab_size;

int fd_alloc(void) {
	int i;

	mutex_spin(&_fdtab_mutex);

	for (i = 4; i < _fdtab_size; i++) {
		if (_fdtab[i].rp == RP_NULL) {
			mutex_free(&_fdtab_mutex);
			return i;
		}
	}

	i = (_fdtab_size < 4) ? 4 : _fdtab_size;

	mutex_free(&_fdtab_mutex);

	return i;
}

int fd_set(int fd, rp_t rp, int mode) {
	int i;

	if (fd < 0) {
		return -1;
	}

	mutex_spin(&_fdtab_mutex);

	if (_fdtab_size <= fd && fd < 4096) {
		_fdtab_size = fd + 1;
		_fdtab = realloc(_fdtab, _fdtab_size * sizeof(struct fdtab));

		for (i = fd; i < _fdtab_size; i++) {
			_fdtab[i].rp = RP_NULL;
		}
	}

	_fdtab[fd].rp = rp;
	_fdtab[fd].mode = mode;

	mutex_free(&_fdtab_mutex);

	return 0;
}

int fd_mode(int fd) {
	int mode;
	
	mutex_spin(&_fdtab_mutex);

	if (fd < 0 || _fdtab_size <= fd) {
		mode = 0;
	}
	else {
		mode = _fdtab[fd].mode;
	}

	mutex_free(&_fdtab_mutex);

	return mode;
}

rp_t fd_rp(int fd) {
	rp_t rp;
	
	mutex_spin(&_fdtab_mutex);

	if (fd < 0 || _fdtab_size <= fd) {
		rp = RP_NULL;
	}
	else {
		rp = _fdtab[fd].rp;
	}

	mutex_free(&_fdtab_mutex);

	return rp;
}

int fd_free(int fd) {
	return fd_set(fd, RP_NULL, 0);
}

int ropen(int fd, rp_t rp, int mode) {
	int old_mode;
	int err;
	rp_t old_rp;

	if (!rp) {
		return fd_set(fd, rp, 0);
	}

	if (fd < 0) fd = fd_alloc();

	mode |= STAT_OPEN;

	old_mode = fd_mode(fd);
	old_rp = fd_rp(fd);

	err = 0;
	if (old_rp == rp) {
		err += rp_setstat(rp, mode);
		err += rp_clrstat(rp, old_mode &~ mode);
	}
	else {
		err += rp_setstat(rp, mode);
		if (old_rp) err += rp_clrstat(old_rp, old_mode);
	}

	if (err) {
		return -err;
	}

	return (fd_set(fd, rp, mode)) ? -1 : fd;
}

int close(int fd) {
	int mode = fd_mode(fd);
	rp_t rp  = fd_rp(fd);

	if (!rp) return -1;

	if (rp_clrstat(rp, mode)) {
		return -1;
	}

	return fd_set(fd, rp, 0);
}

int dup(int fd) {
	return ropen(-1, fd_rp(fd), fd_mode(fd));
}
