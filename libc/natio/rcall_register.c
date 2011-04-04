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

static struct sp_dict *rcall_default;
static void *_rcall_handler(struct msg *msg);

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
		rcall_default = sp_dict_cons(127);
		when(PORT_RCALL, _rcall_handler);
	}

	sp_dict_add(rcall_default, call, (uintptr_t) handler);
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
	char *saveptr;

	// parse arguments
	args = (char*) msg->data;
	argv = 

	handler = (rcall_t) sp_dict_get(rcall_default, 
