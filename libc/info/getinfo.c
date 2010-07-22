/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdlib.h>
#include <mutex.h>
#include <arch.h>
#include <info.h>

const char *getinfo(const char *name) {
	size_t i, n;
	struct info_node *node;

	if (!name) return NULL;

	mutex_spin(&m_info);
	node = &infospace[0];

	for (i = 0; name[i]; i++) {
		n = (size_t) name[i] - 32;
		if (node->next[n] == 0) {
			return NULL;
		}
		node = &infospace[node->next[n]];
	}

	mutex_free(&m_info);

	return &node->value[0];
}
