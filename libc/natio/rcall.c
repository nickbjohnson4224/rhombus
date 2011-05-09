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
#include <stdarg.h>
#include <string.h>
#include <natio.h>
#include <proc.h>
#include <ipc.h>

/*****************************************************************************
 * rcall
 *
 * Generic remote procedure call protocol. Sends a string to the given
 * resource, and recieves a string in return. This function can be used to
 * implement any sort of ad-hoc textual protocol, and is a cover-all for any
 * things that cannot be done with the standard I/O and filesystem routines.
 * Returns NULL on error or empty return string.
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

char *rcall(uint64_t rp, const char *args) {
	struct msg *msg;
	char *rets;

	msg = aalloc(sizeof(struct msg) + strlen(args) + 1, PAGESZ);
	if (!msg) return NULL;
	msg->source = RP_CONS(getpid(), 0);
	msg->target = rp;
	msg->length = strlen(args) + 1;
	msg->port   = PORT_RCALL;
	msg->arch   = ARCH_NAT;
	strcpy((char*) msg->data, args);

	if (msend(msg)) return NULL;
	msg = mwait(PORT_REPLY, rp);

	if (msg->length) {
		rets = strdup((char*) msg->data);
	}
	else {
		rets = NULL;
	}

	free(msg);
	return rets;
}

/*****************************************************************************
 * rcallf
 *
 * Generic remote procedure call protocol. Sends a formatted string to the 
 * given resource, and recieves a string in return. This function can be used 
 * to implement any sort of ad-hoc textual protocol, and is a cover-all for 
 * any things that cannot be done with the standard I/O and filesystem 
 * routines. Returns NULL on error or empty return string.
 */

char *rcallf(uint64_t rp, const char *fmt, ...) {
	va_list ap;
	char *args;
	char *ret;

	// format argument string
	va_start(ap, fmt);
	args = vsaprintf(fmt, ap);
	va_end(ap);

	if (!args) {
		return NULL;
	}

	// perform rcall
	ret = rcall(rp, args);

	// free argument string
	free(args);

	return ret;
}

/****************************************************************************
 * _rcall_map
 *
 * Structure mapping rcall commands to their respective handlers.
 *
 * Notes:
 * Currently, this is implemented as a linked list, which has terrible
 * scalability. If rcall becomes a bottleneck, which it probably will soon,
 * this could be vastly improved by using a trie or PATRICIA tree.
 */

static struct _rcall_map {
	struct _rcall_map *next;

	rcall_t handler;
	char *key;
} *_rcall_map = NULL;

static void _rcall_handler(struct msg *msg);

/****************************************************************************
 * rcall_set
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

int rcall_set(const char *call, rcall_t handler) {
	struct _rcall_map *node;
	
	if (!_rcall_map) {
		when(PORT_RCALL, _rcall_handler);
	}

	node = malloc(sizeof(struct _rcall_map));
	node->next = _rcall_map;
	node->handler = handler;
	node->key = strdup(call);

	_rcall_map = node;

	return 0;
}

/*****************************************************************************
 * rcall_get
 *
 * Returns the rcall handler currently associated with call <call>. Returns
 * null on failure.
 */

rcall_t rcall_get(const char *call) {
	struct _rcall_map *node;

	// get handler
	for (node = _rcall_map; node; node = node->next) {
		if (node->key && !strcmp(node->key, call)) {
			return node->handler;
		}
	}

	return NULL;
}

/*****************************************************************************
 * rcall_handler
 *
 * Message handler for rcall. See rcall_set and rcall for behavior.
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
	for (argc = 0; argv[argc]; argc++);

	handler = rcall_get(argv[0]);

	if (!handler) {
		return;
	}

	rets = handler(msg->source, RP_INDEX(msg->target), argc, argv);
	if (!rets) rets = strdup("");

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
