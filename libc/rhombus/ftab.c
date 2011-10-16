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

struct ftab_entry {
	uint64_t a;
	uint64_t b;
	int status;
	struct ftab_entry *next;
};

static struct ftab_entry *ftab = NULL;
static bool ftab_mutex;

struct ftab_entry *_ftab_set(struct ftab_entry *curr, rp_t a, rp_t b, int status) {
	struct ftab_entry *new_entry = NULL;

	if (!curr) {
		new_entry = malloc(sizeof(struct ftab_entry));
		new_entry->a = a;
		new_entry->b = b;
		new_entry->status = status;
		new_entry->next = NULL;
	
		return new_entry;
	}

	if (curr->a == a && curr->b == b) {
		curr->status = status;
		return curr;
	}

	curr->next = _ftab_set(curr->next, a, b, status);
	return curr;
}

int ftab_set(rp_t a, rp_t b, int status) {

	mutex_spin(&ftab_mutex);
	ftab = _ftab_set(ftab, a, b, status);
	mutex_free(&ftab_mutex);

	return 0;
}

int ftab_reopen(void) {
	struct ftab_entry *curr;

	mutex_spin(&ftab_mutex);
	for (curr = ftab; curr; curr = curr->next) {
		free(rcalls(curr->b, curr->a, "open %d", curr->status));
	}
	mutex_free(&ftab_mutex);

	return 0;
}
