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
#include <stdio.h>

#include <struct.h>

static uint32_t _hash(const char *key) {
	const uint8_t *key8 = (const uint8_t*) key;
	uint32_t hash = 0;

	while (*key8) {
		hash = (hash << 16) + (hash << 6) - hash + *key8;
		key8++;
	}

	return hash;
}

static struct s_table *_cons(size_t size) {
	struct s_table *table;

	table = calloc(sizeof(struct s_table) + (sizeof(struct s_table_entry) << size), 1);
	table->size = size;

	return table;
}

static void _free(struct s_table *table) {
	struct s_table_entry *entry;
	struct s_table_entry *temp;
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

static void _add(struct s_table *table, const char *field, uint32_t hash, void *data) {
	struct s_table_entry *entry;

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

	entry->next = malloc(sizeof(struct s_table_entry));
	entry->next->next = NULL;
	entry->next->hash = hash;
	entry->next->string = strdup(field);
	entry->next->data = data;
}

static struct s_table *_resize(struct s_table *table, uint32_t size) {
	struct s_table *new_table;
	struct s_table_entry *entry;
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

static void _set(struct s_table *table, const char *field, void *data) {
	struct s_table_entry *entry;
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

	if (entry->hash == hash && !strcmp(entry->string, field)) {
		entry->data = data;
		return;
	}

	while (entry->next) {
		if (entry->next->hash == hash && !strcmp(entry->next->string, field)) {
			entry->next->data = data;
			return;
		}
		entry = entry->next;
	}

	entry->next = malloc(sizeof(struct s_table_entry));
	entry->next->next = NULL;
	entry->next->hash = hash;
	entry->next->string = strdup(field);
	entry->next->data = data;
}

static void _del(struct s_table *table, const char *field) {
	struct s_table_entry *entry;
	struct s_table_entry *temp;
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

static void *_get(struct s_table *table, const char *field) {
	struct s_table_entry *entry;
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

static void _iter(struct s_table *table, void (*func)(const char *, void *)) {
	struct s_table_entry *entry;
	uint32_t index;

	for (index = 0; index < (1 << table->size); index++) {

		entry = &table->table[index];

		if (!entry->string) {
			continue;
		}

		while (entry) {
			func(entry->string, entry->data);
			entry = entry->next;
		}
	}
}

static char *_itostring(uint32_t x) {
	uint8_t *string;
	size_t i;

	string = malloc(6);
	string[5] = 0;
	string[4] = 0;

	for (i = 0; i < 4; i++) {
		string[i] = x & 0xFF;

		if (!x) break;

		x >>= 8;

		if (!string[i]) {
			string[5] |= (1 << i);
			string[i] = 0xFF;
		}
	}

	return (char*) string;
}

/*
 * Interface
 */

struct s_table *s_table_set(struct s_table *table, const char *field, void *data) {

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

void *s_table_get(struct s_table *table, const char *field) {

	if (!table || !field) {
		return NULL;
	}
	
	return _get(table, field);
}

void s_table_free(struct s_table *table) {
	_free(table);
}

struct s_table *s_table_setv(struct s_table *table, void *data, const char *field, ...) {
	char *_field;
	va_list ap;

	va_start(ap, field);
	_field = vsaprintf(field, ap);
	va_end(ap);

	if (!_field) {
		return table;
	}
	
	table = s_table_set(table, _field, data);

	free(_field);

	return table;
}

void *s_table_getv(struct s_table *table, const char *field, ...) {
	char *_field;
	void *data;
	va_list ap;

	va_start(ap, field);
	_field = vsaprintf(field, ap);
	va_end(ap);

	if (!_field) {
		return NULL;
	}

	data = s_table_get(table, _field);

	free(_field);

	return table;
}

void s_table_iter(struct s_table *table, void (*iter)(const char *, void *)) {
	_iter(table, iter);
}

struct s_table *s_table_seti(struct s_table *table, uint32_t field, void *data) {
	char *_field;

	_field = _itostring(field);
	table = s_table_set(table, _field, data);
	free(_field);

	return table;
}

void *s_table_geti(struct s_table *table, uint32_t field) {
	char *_field;
	void *data;

	_field = _itostring(field);
	data = s_table_get(table, _field);
	free(_field);

	return data;
}
