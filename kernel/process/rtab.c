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

#include <process.h>
#include <space.h>
#include <types.h>

struct _refc_bucket {
	struct _refc_bucket *next;
	uint64_t rp;
	uint32_t refc;
};

static uint32_t            _refc_count = 8191; // large prime
static struct _refc_bucket *_refc_table[8191];

/*****************************************************************************
 * _refc_hash
 *
 * Return the index of <rp> in the refc table.
 */

static uint32_t _refc_hash(uint64_t rp) {
	uint32_t hash;

	hash = rp & 0xFFFFFFFF;
	hash = (hash << 16) + (hash << 6) - hash + (uint32_t) (rp >> 32);

	return hash % _refc_count;
}

/*****************************************************************************
 * _refc_get
 *
 * Return the reference count of the robject <rp>.
 */

static uint32_t _refc_get(uint64_t rp) {
	struct _refc_bucket *bucket;

	// find bucket
	bucket = _refc_table[_refc_hash(rp)];
	while (bucket) {
		if (bucket->rp == rp) {
			return bucket->refc;
		}
		bucket = bucket->next;
	}

	// entry not found
	return 0;
}

/*****************************************************************************
 * _refc_del
 *
 * Set the reference count of the robject <rp> to zero by removing it from
 * the refc table.
 */

static void _refc_del(uint64_t rp) {
	struct _refc_bucket *bucket;
	struct _refc_bucket *temp;

	bucket = _refc_table[_refc_hash(rp)];

	// check if first entry is to be deleted
	if (bucket->rp == rp) {
		_refc_table[_refc_hash(rp)] = bucket->next;
		heap_free(bucket, sizeof(struct _refc_bucket));
		return;
	}

	// search bucket list for deletable entry
	while (bucket->next) {
		if (bucket->next->rp == rp) {
			temp = bucket->next;
			bucket->next = temp->next;
			heap_free(temp, sizeof(struct _refc_bucket));
			return;
		}
		bucket = bucket->next;
	}

	// entry not found
	return;
}

/*****************************************************************************
 * _refc_set
 *
 * Set the reference count of the robject <rp> to <refc>.
 */

static void _refc_set(uint64_t rp, uint32_t refc) {
	struct _refc_bucket *bucket;
	uint32_t hash = _refc_hash(rp);

	if (refc == 0) {
		// delete entry instead
		_refc_del(rp);
		return;
	}

	// find bucket
	bucket = _refc_table[hash];
	while (bucket) {
		if (bucket->rp == rp) {
			bucket->refc = refc;
		}
		bucket = bucket->next;
	}

	// entry not found; add new entry
	bucket = heap_alloc(sizeof(struct _refc_bucket));
	bucket->next = _refc_table[hash];
	bucket->rp   = rp;
	bucket->refc = refc;
	_refc_table[hash] = bucket;
}

/*****************************************************************************
 * rtab_free
 *
 * Delete all robject table entries for the process <proc>. Reference counts
 * for all entries are decremented.
 */

void rtab_free(struct process *proc) {
	uint32_t i;

	// traverse table
	for (i = 0; i < proc->rtab_count; i++) {

		if (proc->rtab[i]) {

			// decrement reference count for entry
			_refc_set(proc->rtab[i], _refc_get(proc->rtab[i]) - 1);
		}
	}

	if (proc->rtab) heap_free(proc->rtab, proc->rtab_count * sizeof(uint64_t));
	proc->rtab = NULL;
	proc->rtab_count = 0;
}

/*****************************************************************************
 * rtab_close
 *
 * Delete the robject table entry for the robject <rp> in the process <proc>.
 * The reference count for the robject <rp> is decremented.
 */

void rtab_close(struct process *proc, uint64_t rp) {
	uint32_t i;

	// find slot
	for (i = 0; i < proc->rtab_count; i++) {
		
		if (proc->rtab[i] == rp) {
			// found slot
			proc->rtab[i] = 0;
			_refc_set(rp, _refc_get(rp) - 1);
			return;
		}
	}

	// could not find slot
	return;
}

/*****************************************************************************
 * rtab_grant
 *
 * Add a robject table entry for the robject <rp> to the process <proc>. The
 * reference count for the robject <rp> is incremented.
 */

void rtab_grant(struct process *proc, uint64_t rp) {
	uint32_t count;
	uint64_t *temp;
	uint64_t *temp2;
	uint32_t i;

	// find empty slot for entry
	for (i = 0; i < proc->rtab_count; i++) {

		if (proc->rtab[i] == rp) {
			// already contains this robject
			return;
		}

		if (proc->rtab[i] == 0) {
			// found suitable entry
			proc->rtab[i] = rp;
			_refc_set(rp, 1 + _refc_get(rp));
			return;
		}
	}

	// could not find slot; reallocate table
	count = proc->rtab_count + 16;
	temp = heap_alloc(count * sizeof(uint64_t));

	// copy contents to new table
	for (i = 0; i < proc->rtab_count; i++) {	
		temp[i] = proc->rtab[i];
	}

	// clear remainder of table
	for (; i < count; i++) {
		temp[i] = 0;
	}

	// free old table
	temp2 = proc->rtab;
	proc->rtab = temp;
	if (temp2) heap_free(temp2, proc->rtab_count * sizeof(uint64_t));
	proc->rtab_count = count;

	// add new entry
	rtab_grant(proc, rp);
}

/*****************************************************************************
 * rtab_count
 *
 * Returns the reference count of the given robject <rp>.
 */

uint32_t rtab_count(uint64_t rp) {
	return _refc_get(rp);
}
