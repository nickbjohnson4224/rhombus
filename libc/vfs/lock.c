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

	if (lock->shlock) {
		vfs_lock_list_free(lock->shlock);
	}

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
		lock->shlock = vfs_lock_list_del(lock->shlock, pid);
		break;
	case LOCK_EX:
		lock->exlock = 0;
		break;
	case LOCK_PR:
		lock->prlock = 0;
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
		if (lock->exlock || lock->prlock) {
			err = 1;
		}
		else {
			lock->shlock = vfs_lock_list_add(lock->shlock, pid);
		}
		break;
	case LOCK_EX:
		if (lock->shlock || lock->exlock || lock->prlock) {
			err = 1;
		}
		else {
			lock->exlock = pid;
		}
		break;
	case LOCK_PR:
		if (lock->shlock || lock->exlock || lock->prlock) {
			err = 1;
		}
		else {
			lock->prlock = pid;
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

	if (vfs_lock_list_tst(lock->shlock, pid)) {
		mutex_free(&lock->mutex);
		return LOCK_SH;
	}

	if (lock->exlock == pid) {
		mutex_free(&lock->mutex);
		return LOCK_EX;
	}

	if (lock->prlock == pid) {
		mutex_free(&lock->mutex);
		return LOCK_PR;
	}
	
	mutex_free(&lock->mutex);
	return LOCK_UN;
}

struct vfs_lock_list *vfs_lock_list_add(struct vfs_lock_list *ll, uint32_t pid) {
	struct vfs_lock_list *new;

	new = malloc(sizeof(struct vfs_lock_list));
	new->pid = pid;
	new->next = ll;

	return new;
}

struct vfs_lock_list *vfs_lock_list_del(struct vfs_lock_list *ll, uint32_t pid) {
	struct vfs_lock_list *next;
	
	if (!ll) {
		return NULL;
	}

	if (ll->pid == pid) {
		next = ll->next;
		free(ll);
		return ll->next;
	}

	ll->next = vfs_lock_list_del(ll->next, pid);

	return ll;
}

int vfs_lock_list_tst(struct vfs_lock_list *ll, uint32_t pid) {
	
	if (!ll) {
		return 0;
	}

	if (ll->pid == pid) {
		return 1;
	}

	return vfs_lock_list_tst(ll->next, pid);
}

struct vfs_lock_list *vfs_lock_list_free(struct vfs_lock_list *ll) {
	
	if (!ll) {
		return NULL;
	}

	vfs_lock_list_free(ll->next);
	free(ll);
	return NULL;
}
