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

#include <stdlib.h>
#include <mutex.h>
#include <proc.h>
#include <vfs.h>

struct vfs_lock *vfs_lock_cons(void) {
	return calloc(sizeof(struct vfs_lock), 1);
}

void vfs_lock_free(struct vfs_lock *lock) {
	
	mutex_spin(&lock->mutex);

	id_hash_free(&lock->shlock);

	free(lock);
}

int vfs_lock_acquire(struct vfs_lock *lock, uint32_t pid, int locktype) {
	int err = 0;
	int oldtype;

	oldtype = vfs_lock_current(lock, pid);

	if (oldtype == locktype) {
		return 0;
	}

	mutex_spin(&lock->mutex);

	/* release old lock */
	switch (oldtype) {
	case LOCK_UN: break;
	case LOCK_SH:
		id_hash_set(&lock->shlock, pid, 0);
		break;
	case LOCK_EX:
		lock->exlock = 0;
		break;
	default:
		err = 1;
	}

	if (err) {
		mutex_free(&lock->mutex);
		return err;
	}

	/* acquire the new lock */
	switch (locktype) {
	case LOCK_UN: break;
	case LOCK_SH:
		if (lock->exlock) {
			err = 1;
		}
		else {
			id_hash_set(&lock->shlock, pid, 1);
		}
		break;
	case LOCK_EX:
		if (id_hash_count(&lock->shlock) || lock->exlock) {
			err = 1;
		}
		else {
			lock->exlock = pid;
		}
		break;
	default:
		err = 1;
	}

	mutex_free(&lock->mutex);

	return err;
}

int vfs_lock_current(struct vfs_lock *lock, uint32_t pid) {
	
	mutex_spin(&lock->mutex);

	if (id_hash_get(&lock->shlock, pid)) {
		mutex_free(&lock->mutex);
		return LOCK_SH;
	}

	if (lock->exlock == pid) {
		mutex_free(&lock->mutex);
		return LOCK_EX;
	}

	mutex_free(&lock->mutex);
	return LOCK_UN;
}
