/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <ipc.h>

#include <stdio.h>
#include <natio.h>
#include <string.h>
#include <stdlib.h>

#include "tarfs.h"

/****************************************************************************
 * tarfs_info
 *
 * PORT_INFO handler.
 */

void tarfs_info(uint32_t source, struct packet *packet) {
	struct info_query *query;
	
	/* reject null packets */
	if (!packet) {
		psend(PORT_REPLY, source, NULL);
		return;
	}

	query = pgetbuf(packet);

	if (!strcmp(query->field, "size")) {

		/* get file size */
		if (packet->target_inode == 0) {
			strcpy(query->value, "");
		}
		else {
			sprintf(query->value, "%d", inode[packet->target_inode].size);
		}
	}
	else {
		strcpy(query->value, "");
	}

	psend(PORT_REPLY, source, packet);
	pfree(packet);
}
