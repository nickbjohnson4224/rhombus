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
	int mode;		// file connection status
	rp_t rp;		// file resource pointer
	uint64_t pos;	// file position
};

static struct fdtab *_fdtab = NULL;
static bool  _fdtab_mutex   = false;
static int   _fdtab_size    = 0;

int fd_alloc(void) {
	int i;

	mutex_spin(&_fdtab_mutex);

	// find the smallest open file descriptor with index >= 4
	for (i = 4; i < _fdtab_size; i++) {
		if (_fdtab[i].rp == RP_NULL) {
			mutex_free(&_fdtab_mutex);
			return i;
		}
	}

	// if table is not large enough, default to 4
	i = (_fdtab_size < 4) ? 4 : _fdtab_size;

	mutex_free(&_fdtab_mutex);

	return i;
}

int fd_set(int fd, rp_t rp, int mode) {
	int i;

	if (fd < 0 || fd >= 4096) {
		// file descriptor out of range
		return -1;
	}

	mutex_spin(&_fdtab_mutex);

	if (_fdtab_size <= fd) {

		// reallocate table
		_fdtab_size = fd + 1;
		_fdtab = realloc(_fdtab, _fdtab_size * sizeof(struct fdtab));

		// clear remainder of table
		for (i = fd; i < _fdtab_size; i++) {
			_fdtab[i].rp = RP_NULL;
		}
	}

	// set table entry
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

int fd_seek(int fd, uint64_t pos) {
	
	mutex_spin(&_fdtab_mutex);

	if (fd >= 0 && _fdtab_size > fd) {
		_fdtab[fd].pos = pos;
	
		mutex_free(&_fdtab_mutex);
		return 0;
	}

	mutex_free(&_fdtab_mutex);

	return -1;
}

uint64_t fd_pos(int fd) {
	uint64_t pos;

	mutex_spin(&_fdtab_mutex);

	if (fd < 0 || _fdtab_size <= fd) {
		pos = 0;
	}
	else {
		pos = _fdtab[fd].pos;
	}

	mutex_free(&_fdtab_mutex);

	return pos;
}
