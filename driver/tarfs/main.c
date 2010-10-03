/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
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

#include <mutex.h>
#include <proc.h>
#include <ipc.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <natio.h>

#include "tarfs.h"

struct tarfs_inode inode[256];

bool m_parent;
FILE *parent;

char name[100];
char root[100];

/****************************************************************************
 * getname
 *
 * Copies the last part of a path into the buffer name.
 */

static void getname(char *name, char *path) {
	int i;
	
	for (i = strlen(path); i >= 0; i--) {
		if (path[i] == '/') {
			i++;
			break;
		}
	}

	strcpy(name, &path[i]);
}

/****************************************************************************
 * tarfs - tape archive filesystem driver
 *
 * Usage: tarfs [parent]
 *
 * Constructs a filesystem using the tarfile parent as a parent driver.
 */

int main(int argc, char **argv) {

	/* reject if no parent is speicified */
	if (argc < 2) {
		fprintf(stderr, "%s: no parent driver specified", argv[0]);

		return 1;
	}

	/* figure out name */
	getname(name, argv[1]);
	strcat (name, ".");
	strcat (name, argv[0]);

	if (argc >= 3) {
		/* root is specified */
		strcpy(root, argv[2]);
	}
	else {
		/* root is implicit */
		strcpy(root, "/");
		strcat(root, name);
	}

	/* get parent driver stream */
	parent = fopen(argv[1], "r");

	if (!parent) {
		/* parent does not exist - fail */
		fprintf(stderr, "%s: no parent driver %s\n", argv[0], argv[1]);

		return 1;
	}

	/* initialize tarfs on parent driver */
	tarfs_init();

	/* register handlers */
	when(PORT_READ, tarfs_read);

	/* synchronize with parent process */
	psend(PORT_CHILD, getppid(), NULL);

	/* daemonize */
	lfs_event_start();
	_done();

	return 0;
}
