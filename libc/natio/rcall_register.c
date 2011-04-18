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

#include <stdlib.h>
#include <string.h>
#include <natio.h>
#include <proc.h>
#include <ipc.h>

#include "dict.h"

static struct sh_dict *rcall_default;
static void _rcall_handler(struct msg *msg);

/****************************************************************************
 * rcall_register
 *
 * Register a handler to be called if a specific rcall message is recieved.
 * If the string <call> is the first whitespace-delimited substring of the
 * recieved message, the given handler is used. If no handlers are registered
 * for a recieved message, it is discarded.
 *
 * The handler is given the resource pointer of the message source, the index
 * of its target, and a main()-like argument list of the parsed message 
 * contents. argv[0] is the name of the call, just like argv[0] is the name of
 * the invoked command for main().
 *
 * The string the handler returns is returned from rcall() in the calling
 * process.
 *
 * Returns zero on success, nonzero on error.
 */

int rcall_register(const char *call, rcall_t handler) {
	
	if (!rcall_default) {
		rcall_default = sh_dict_cons(127);
		when(PORT_RCALL, _rcall_handler);
	}

	sh_dict_add(rcall_default, call, (handler_t) handler);

	return 1;
}

/*****************************************************************************
 * rcall_handler
 *
 * Message handler for rcall. See rcall_register for behavior.
 *
 * protocol:
 *   port: PORT_RCALL
 *
 *   request:
 *     char args[]
 *
 *   reply:
 *     char rets[]
 */

void _rcall_handler(struct msg *msg) {
	struct msg *reply;
	char *args;
	char *rets;
	rcall_t handler;
	int argc;
	char **argv;

	// parse arguments
	args = (char*) msg->data;
	argv = strparse(args, " ");
	for (argc = 0; argv[argc]; argv++);

	handler = sh_dict_get(rcall_default, argv[0]);

	if (!handler) {
		return;
	}

	rets = handler(msg->source, RP_INDEX(msg->target), argc, argv);

	for (argc = 0; argv[argc]; argv++) {
		free(argv[argc]);
	}

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
