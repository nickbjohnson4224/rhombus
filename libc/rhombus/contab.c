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

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <natio.h>
#include <mutex.h>

/*****************************************************************************
 * Connection Table
 */

struct contab {
	int status_count[32];
	int refc;
	rp_t rp;
};

static struct contab *_contab;
static bool _contab_mutex;
static size_t _contab_size;

static int _contab_add(rp_t rp, int status);
static int _contab_del(rp_t rp, int status);
static int _contab_get(rp_t rp);

static int _rp_connect(rp_t rp, int status);

static int _contab_add(rp_t rp, int status) {
	size_t i;
	size_t j;

	if (!status) return 0;

	mutex_spin(&_contab_mutex);

	for (i = 0; i < _contab_size; i++) {
		if (_contab[i].rp == RP_NULL) {
			// found empty entry; convert

			_contab[i].rp = rp;
			_contab[i].refc = 0;

			for (j = 0; j < 32; j++) {
				_contab[i].status_count[j] = 0;
			}
		}

		if (_contab[i].rp == rp) {
			// found existing (or just converted) entry; use

			// increment status counts
			for (j = 0; j < 32; j++) {
				if (status & (1 << j)) {
					_contab[i].status_count[j]++;
				}
			}

			// increment reference count
			_contab[i].refc++;

			mutex_free(&_contab_mutex);

			// reopen connection
			if (_rp_connect(rp, _contab_get(rp))) {
				_contab_del(rp, status);
				return 1;
			}

			return 0; // success
		}
	}

	// no entry found; regrow table
	_contab = realloc(_contab, sizeof(struct contab) * (_contab_size + 16));

	// clear new entries
	for (i = _contab_size; i < _contab_size + 16; i++) {
		_contab[i].rp = RP_NULL;
	}
	_contab_size += 16;

	// use first new entry
	i = _contab_size - 16;

	_contab[i].rp = rp;
	_contab[i].refc = 1;

	// set status counts
	for (j = 0; j < 32; j++) {
		if (status & (1 << j)) {
			_contab[i].status_count[j] = 1;
		}
		else {
			_contab[i].status_count[j] = 0;
		}
	}

	mutex_free(&_contab_mutex);

	// reopen connection
	if (_rp_connect(rp, _contab_get(rp))) {
		_contab_del(rp, status);
		return 1;
	}

	return 0;
}

static int _contab_del(rp_t rp, int status) {
	size_t i;
	size_t j;

	if (!status) return 0;

	mutex_spin(&_contab_mutex);

	for (i = 0; i < _contab_size; i++) {
		if (_contab[i].rp == rp) {
			// found entry
			
			// decrement status counts
			for (j = 0; j < 32; j++) {
				if (status & (1 << j)) {
					_contab[i].status_count[j]--;
				}
			}

			// decrement reference count
			_contab[i].refc--;

			if (_contab[i].refc <= 0) {
				// remove entry
				_contab[i].rp = RP_NULL;
			}

			mutex_free(&_contab_mutex);
			
			// reopen/close connection
			if (_rp_connect(rp, _contab_get(rp))) {
				_contab_del(rp, status);
				return 1;
			}

			return 0;
		}
	}

	mutex_free(&_contab_mutex);

	return 1;
}

static int _contab_get(rp_t rp) {
	int status = 0;
	size_t i;
	size_t j;

	mutex_spin(&_contab_mutex);

	for (i = 0; i < _contab_size; i++) {
		if (_contab[i].rp == rp) {
			// found entry

			// read status counts
			for (j = 0; j < 32; j++) {
				if (_contab[i].status_count[j] > 0) {
					status |= (1 << j);
				}
			}

			break;
		}
	}

	mutex_free(&_contab_mutex);

	return status;
}

int __reconnect(void) {
	int status;
	size_t i;
	size_t j;

	mutex_spin(&_contab_mutex);

	for (i = 0; i < _contab_size; i++) {
		if (_contab[i].rp != RP_NULL) {
			// found active entry

			status = 0;
			for (j = 0; j < 32; j++) {
				if (_contab[i].status_count[j] > 0) {
					status |= (1 << j);
				}
			}

//			_rp_connect(_contab[i].rp, status);
		}
	}

	mutex_free(&_contab_mutex);

	return 0;
}

static int _rp_connect(rp_t rp, int status) {
	char *reply;

	if (status) {
		reply = rcall(rp, "open %d", status);
	}
	else {
		reply = rcall(rp, "close");
	}

	if (!reply) {
		errno = ENOSYS;
		return 1;
	}

	if (reply[0] == '!') {
		if      (!strcmp(reply, "! nosys"))		errno = ENOSYS;
		else if (!strcmp(reply, "! denied"))	errno = EACCES;
		else if (!strcmp(reply, "! nfound"))	errno = ENOENT;
		else									errno = EUNK;
		free(reply);
		return 1;
	}
	
	free(reply);
	return 0;
}

/*****************************************************************************
 * rp_getstat
 *
 * Return the state of the connection to <rp> as registered in the connection
 * table.
 */

int rp_getstat(rp_t rp) {
	return _contab_get(rp);
}

/*****************************************************************************
 * rp_setstat
 *
 * Create a new connection with status <status> to <rp> in the connection
 * table. The real connection will have a status that is a superset of 
 * <status> until this connection is removed.
 */

int rp_setstat(rp_t rp, int status) {
	return _contab_add(rp, status | STAT_OPEN);
}

/*****************************************************************************
 * rp_clrstat
 *
 * Remove a connection with status <status> from the connection to <rp>.
 */

int rp_clrstat(rp_t rp, int status) {
	return _contab_del(rp, status | STAT_OPEN);
}
