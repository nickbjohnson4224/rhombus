/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "dict.h"

void dict_add(struct dict_node *dict, const char *key, const char *value) {

	if (dict == NULL) {
		return;
	}

	dict->refc++;

	while (dict->link) {
		dict = dict->link;
		dict->refc++;
	}

	if (key[0] == '\0') {
		dict->value = malloc(strlen(value) + 1);
		strcpy(dict->value, value);
	}
	else {
		if (dict->nexti && dict->immediate == key[0]) {
			dict_add(dict->nexti, &key[1], value);
		}
		else if (dict->nextv) {
			if (!dict->nextv[(size_t) key[0]]) {
				dict->nextv[(size_t) key[0]] = calloc(sizeof(struct dict_node), 1);
			}
			dict_add(dict->nextv[(size_t) key[0]], &key[1], value);
		}
		else if (!dict->nexti) {
			dict->nexti = calloc(sizeof(struct dict_node), 1);
			dict->immediate = key[0];
			dict_add(dict->nexti, &key[1], value);
		}
		else {
			dict->nextv = calloc(sizeof(struct dict_node*), 256);
			if (dict->nexti) {
				dict->nextv[(size_t) dict->immediate] = dict->nexti;
				dict->nexti = NULL;
			}
			dict->nextv[(size_t) key[0]] = calloc(sizeof(struct dict_node), 1);
			dict_add(dict->nextv[(size_t) key[0]], &key[1], value);
		}
	}
}

const char *dict_get(struct dict_node *dict, const char *key) {

	if (dict == NULL) {
		return NULL;
	}

	while (dict->link) {
		dict = dict->link;
	}

	if (key[0] == '\0') {
		return dict->value;
	}
	else {
		if (dict->nexti && (dict->immediate == key[0])) {
			return dict_get(dict->nexti, &key[1]);
		}
		else if (dict->nextv && dict->nextv[(size_t) key[0]]) {
			return dict_get(dict->nextv[(size_t) key[0]], &key[1]);
		}
		else {
			return NULL;
		}
	}
}
