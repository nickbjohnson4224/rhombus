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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <robject.h>

/*
 * hash table implementation
 */

static uint32_t _hash(const char *key) {
	const uint8_t *key8 = (const uint8_t*) key;
	uint32_t hash = 0;

	while (*key8) {
		hash = (hash << 16) + (hash << 6) - hash + *key8;
		key8++;
	}

	return hash;
}

static struct __robject_data_table *_cons(size_t size) {
	struct __robject_data_table *table;

	table = calloc(
		sizeof(struct __robject_data_table) + 
		(sizeof(struct __robject_data_table_entry) << size),
		1);
	
	table->size = size;

	return table;
}

static void _free(struct __robject_data_table *table) {
	struct __robject_data_table_entry *entry;
	struct __robject_data_table_entry *temp;
	size_t i;

	for (i = 0; i < (1 << table->size); i++) {
		entry = &table->table[i];
		if (entry->string) {
			free(entry->string);
			entry = entry->next;
			while (entry) {
				free(entry->string);
				temp = entry->next;
				free(entry);
				entry = temp;
			}
		}
	}

	free(table);
}

static void _add(struct __robject_data_table *table, const char *field, uint32_t hash, void *data) {
	struct __robject_data_table_entry *entry;

	entry = &table->table[hash & ((1 << table->size) - 1)];

	if (!entry->string) {
		entry->string = strdup(field);
		entry->data = data;
		entry->hash = hash;
		table->load++;
		return;
	}

	while (entry->next) {
		entry = entry->next;
	}

	entry->next = malloc(sizeof(struct __robject_data_table_entry));
	entry->next->next = NULL;
	entry->next->hash = hash;
	entry->next->string = strdup(field);
	entry->next->data = data;
}

static struct __robject_data_table *_resize(struct __robject_data_table *table, uint32_t size) {
	struct __robject_data_table *new_table;
	struct __robject_data_table_entry *entry;
	size_t i;

	new_table = _cons(size);

	for (i = 0; i < (1 << table->size); i++) {
		entry = &table->table[i];
		if (entry->string) {
			while (entry) {
				_add(new_table, entry->string, entry->hash, entry->data);
				entry = entry->next;
			}
		}
	}

	_free(table);

	return new_table;
}

static void _set(struct __robject_data_table *table, const char *field, void *data) {
	struct __robject_data_table_entry *entry;
	uint32_t hash;
	
	hash = _hash(field);
	entry = &table->table[hash & ((1 << table->size) - 1)];

	if (!entry->string) {
		entry->string = strdup(field);
		entry->data = data;
		entry->hash = hash;
		table->load++;
		return;
	}

	while (entry->next) {
		if (entry->next->hash == hash && !strcmp(entry->next->string, field)) {
			entry->next->data = data;
			return;
		}
		entry = entry->next;
	}

	entry->next = malloc(sizeof(struct __robject_data_table_entry));
	entry->next->next = NULL;
	entry->next->hash = hash;
	entry->next->string = strdup(field);
	entry->next->data = data;
}

static void _del(struct __robject_data_table *table, const char *field) {
	struct __robject_data_table_entry *entry;
	struct __robject_data_table_entry *temp;
	uint32_t hash;
	
	hash = _hash(field);
	entry = &table->table[hash & ((1 << table->size) - 1)];

	if (!entry->string) {
		return;
	}

	if (entry->hash == hash && !strcmp(entry->string, field)) {
		if (entry->next) {
			free(entry->string);
			temp = entry->next;
			entry->string = temp->string;
			entry->hash = temp->hash;
			entry->data = temp->data;
			entry->next = temp->next;
			free(temp);
		}
		else {
			free(entry->string);
			entry->string = NULL;
			table->load--;
		}
		return;
	}

	while (entry->next) {
		if (entry->next->hash == hash && !strcmp(entry->next->string, field)) {
			temp = entry->next;
			entry->next = temp->next;
			free(temp->string);
			free(temp);
			return;
		}
		entry = entry->next;
	}
}

static void *_get(struct __robject_data_table *table, const char *field) {
	struct __robject_data_table_entry *entry;
	uint32_t hash;

	hash = _hash(field);
	entry = &table->table[hash & ((1 << table->size) - 1)];

	if (!entry->string) {
		return NULL;
	}

	while (entry) {
		if (entry->hash == hash && !strcmp(entry->string, field)) {
			return entry->data;
		}
		entry = entry->next;
	}

	return NULL;
}

/*
 * Interface
 */

struct __robject_data_table *
__data_table_set(struct __robject_data_table *table, const char *field, void *data) {

	if (!field) return table;

	if (!table) {
		table = _cons(6);
	}

	if (data) {
		_set(table, field, data);
		if (table->load > (3U << table->size) >> 2) {
			// load exceeds 75% - double size
			table = _resize(table, table->size + 1);
		}
	}
	else {
		_del(table, field);
		if (table->size > 6 && table->load < (1U << table->size) >> 3) {
			// load is less than 12.5% and there are more than 64 slots - halve size
			table = _resize(table, table->size - 1);
		}
	}

	return table;
}

void *__data_table_get(struct __robject_data_table *table, const char *field) {
	if (!table) return NULL;
	if (!field) return NULL;
	return _get(table, field);
}

void __data_table_free(struct __robject_data_table *table) {
	_free(table);
}
