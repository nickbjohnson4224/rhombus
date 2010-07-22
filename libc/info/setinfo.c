/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdlib.h>
#include <mutex.h>
#include <arch.h>
#include <info.h>
#include <string.h>

struct info_node *infospace = (void*) INFO_TABLE;
bool m_info;

void setinfo(const char *name, const char *value) {
	size_t i, n;
	struct info_node *node;

	if (!name || !value) return;

	mutex_spin(&m_info);
	node = &infospace[0];

	for (i = 0; name[i]; i++) {
		n = (size_t) name[i] - 32;
		if (node->next[n] == 0) {
			node->next[n] = info_node_alloc();
		}
		node = &infospace[node->next[n]];
	}

	for (i = 0; value[i]; i++) {
		node->value[i] = value[i];
	}
	node->value[i] = '\0';

	mutex_free(&m_info);
}
