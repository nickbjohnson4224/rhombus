/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <arch.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <proc.h>
#include <ipc.h>
#include <exec.h>
#include <stdio.h>
#include <natio.h>
#include <pack.h>
#include <abi.h>

/****************************************************************************
 * reject/ignore
 *
 * Actions to be taken on the reception of an unwanted event.
 */

static void reject(struct msg *msg) {
	merror(msg);
}

static void ignore(struct msg *msg) {
	free(msg);
}

/****************************************************************************
 * _init
 *
 * Function called at beginning of all processes, used to initialize the C
 * library.
 */

void _init() {
	extern int main(int argc, char **argv);
	extern void _on_event(void);
	char **argv, *pack;
	size_t length;
	int argc;

	_when((uintptr_t) _on_event);

	/* unpack environment variables */
	__loadenv();

	/* setup standard streams */
	stdin   = fdopen(fdload(0), "r");
	stdout  = fdopen(fdload(1), "w");
	stderr  = fdopen(fdload(2), "w");
	fs_root = fdload(3);

	/* set up signals */
	__sig_init();
	when(PORT_CHILD, NULL);

	/* set up I/O handlers */
	when(PORT_REPLY, NULL);
	when(PORT_READ,  reject);
	when(PORT_WRITE, reject);
	when(PORT_SYNC,	 reject);
	when(PORT_RESET, reject);
	when(PORT_SHARE, reject);
	when(PORT_RCALL, reject);
	when(PORT_EVENT, ignore);

	/* unpack argument list */
	pack = __pack_load(PACK_KEY_ARG, &length);
	if (pack) {
		argv = loadarg(pack);
		for (argc = 0; argv[argc]; argc++);
		setname(path_name(argv[0]));
	}
	else {
		argv = NULL;
		argc = 0;
		setname("unknown");
	}

	__pack_reset();

	/* execute main program */
	exit(main(argc, argv));
}
