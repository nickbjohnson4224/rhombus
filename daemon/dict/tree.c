/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "dict.h"

static struct dict_node *dict_find(struct dict_node *dict, const char *key) {

	if (dict == NULL) {
		return NULL;
	}

	while (dict->link) {
		dict = dict->link;
	}

	if (key[0] == '\0') {
		return dict;
	}
	else {
		if (dict->nexti && (dict->immediate == key[0])) {
			return dict_find(dict->nexti, &key[1]);
		}
		else if (dict->nextv && dict->nextv[(size_t) key[0]]) {
			return dict_find(dict->nextv[(size_t) key[0]], &key[1]);
		}
		else {
			return NULL;
		}
	}
}

static struct dict_node *dict_inst(struct dict_node *dict, const char *key) {

	if (dict == NULL) {
		return NULL;
	}

	dict->refc++;

	while (dict->link) {
		dict = dict->link;
		dict->refc++;
	}

	if (key[0] == '\0') {
		return dict;
	}
	else {
		if (dict->nexti && dict->immediate == key[0]) {
			return dict_inst(dict->nexti, &key[1]);
		}
		else if (dict->nextv) {
			if (!dict->nextv[(size_t) key[0]]) {
				dict->nextv[(size_t) key[0]] = calloc(sizeof(struct dict_node), 1);
			}
			return dict_inst(dict->nextv[(size_t) key[0]], &key[1]);
		}
		else if (!dict->nexti) {
			dict->nexti = calloc(sizeof(struct dict_node), 1);
			dict->immediate = key[0];
			return dict_inst(dict->nexti, &key[1]);
		}
		else {
			dict->nextv = calloc(sizeof(struct dict_node*), 256);
			if (dict->nexti) {
				dict->nextv[(size_t) dict->immediate] = dict->nexti;
				dict->nexti = NULL;
			}
			dict->nextv[(size_t) key[0]] = calloc(sizeof(struct dict_node), 1);
			return dict_inst(dict->nextv[(size_t) key[0]], &key[1]);
		}
	}
}

int dict_add(struct dict_node *dict, const char *key, const char *value) {
	dict = dict_inst(dict, key);

	if (!dict) return -1;

	dict->value = malloc(strlen(value) + 1);
	strcpy(dict->value, value);

	return 0;
}

const char *dict_get(struct dict_node *dict, const char *key) {
	return dict_find(dict, key)->value;
}

int dict_lnk(struct dict_node *dict, const char *key, const char *link) {
	struct dict_node *src, *dest;

	dest = dict_find(dict, link);
	if (!dest) return 1;
	
	src = dict_inst(dict, key);
	if (!src) return 1;

	src->link = dest;
	dest->refc++;

	return 0;
}
