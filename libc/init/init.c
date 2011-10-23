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

#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>

#include <rho/natio.h>
#include <rho/pack.h>
#include <rho/arch.h>
#include <rho/proc.h>
#include <rho/exec.h>
#include <rho/abi.h>
#include <rho/ipc.h>

/****************************************************************************
 * __reject/__ignore
 *
 * Actions to be taken on the reception of an unwanted message.
 */

static void __reject(struct msg *msg) {
	merror(msg);
}

static void __ignore(struct msg *msg) {
	free(msg);
}

/****************************************************************************
 * __rcall_handler
 *
 * Action to be taken on reception of an rcall.
 */

static void __rcall_handler(struct msg *msg) {
	struct msg *reply;
	char *rets;

	rets = rcall_call(msg->source, (const char*) msg->data);
	if (!rets) rets = strdup("");

	reply = aalloc(sizeof(struct msg) + strlen(rets) + 1, PAGESZ);
	reply->source = msg->target;
	reply->target = msg->source;
	reply->length = strlen(rets) + 1;
	reply->port   = PORT_REPLY;
	reply->arch   = ARCH_NAT;
	strcpy((char*) reply->data, rets);
	free(rets);

	free(msg);
	msend(reply);
}

/*****************************************************************************
 * __ping
 *
 * Default action for "ping" rcall.
 */

static char *__ping(rp_t source, int argc, char **argv) {
	return strdup("T");
}

/*****************************************************************************
 * __name
 *
 * Default action for "name" rcall.
 */

static char *__name(rp_t source, int argc, char **argv) {
	return strvcat("[", getname_s(), "]", NULL);
}

/*****************************************************************************
 * __type
 *
 * Default action for "type" rcall.
 */

static char *__type(rp_t source, int argc, char **argv) {
	return strdup("proc");
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
	stdin   = fdopen(ropen(0, fdload(0), 0), "r");
	stdout  = fdopen(ropen(1, fdload(1), 0), "w");
	stderr  = fdopen(ropen(2, fdload(2), 0), "w");
	fs_root = fdload(3);

	setvbuf(stdout, NULL, _IOLBF, BUFSIZ);

	/* set up signals */
	__sig_init();
	when(PORT_CHILD, NULL);

	/* set up I/O handlers */
	when(PORT_REPLY, NULL);
	when(PORT_READ,  __reject);
	when(PORT_WRITE, __reject);
	when(PORT_SYNC,	 __reject);
	when(PORT_RESET, __reject);
	when(PORT_SHARE, __reject);
	when(PORT_RCALL, __rcall_handler);
	when(PORT_EVENT, __ignore);
	when(PORT_CLOSE, __ignore);

	/* set up basic rcall handlers */
	rcall_hook("ping", __ping);
	rcall_hook("type", __type);
	rcall_hook("name", __name);

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
