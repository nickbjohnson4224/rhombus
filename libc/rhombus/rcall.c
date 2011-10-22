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

#include <rho/struct.h>
#include <rho/natio.h>
#include <rho/proc.h>
#include <rho/ipc.h>

/*****************************************************************************
 * __rcall
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

static char *__rcall(uint64_t rp, const char *args) {
	struct msg *msg;
	size_t length;
	char *rets;

	length = strlen(args) + 1;

	msg = aalloc(sizeof(struct msg) + length, PAGESZ);
	if (!msg) return NULL;
	msg->source = RP_CURRENT_THREAD;
	msg->target = rp;
	msg->length = length;
	msg->port   = PORT_RCALL;
	msg->arch   = ARCH_NAT;
	memcpy(msg->data, args, length);

	if (msend(msg)) return NULL;
	msg = mwait(PORT_REPLY, rp);

	if (msg->length && msg->data[0] != '\0') {
		rets = strdup((char*) msg->data);
	}
	else {
		rets = NULL;
	}

	free(msg);
	return rets;
}

/*****************************************************************************
 * rcall
 *
 * Generic remote procedure call protocol. Sends a formatted string to the 
 * given resource, and recieves a string in return. This function can be used 
 * to implement any sort of ad-hoc textual protocol, and is a cover-all for 
 * any things that cannot be done with the standard I/O and filesystem 
 * routines. Returns NULL on error or empty return string.
 */

char *rcall(rp_t rp, const char *fmt, ...) {
	va_list ap;
	char *args;
	char *ret;

	if (strchr(fmt, '%')) {

		// format argument string
		va_start(ap, fmt);
		args = vsaprintf(fmt, ap);
		va_end(ap);

		if (!args) {
			return NULL;
		}

		// perform rcall
		ret = __rcall(rp, args);

		// free argument string
		free(args);
	}
	else {
		
		// just use the format string
		ret = __rcall(rp, fmt);
	}

	return ret;
}

char *frcall(int fd, const char *fmt, ...) {
	va_list ap;
	char *args;
	char *ret;
	rp_t rp;

	rp = fd_rp(fd);

	if (strchr(fmt, '%')) {

		// format argument string
		va_start(ap, fmt);
		args = vsaprintf(fmt, ap);
		va_end(ap);

		if (!args) {
			return NULL;
		}

		// perform rcall
		ret = __rcall(rp, args);

		// free argument string
		free(args);
	}
	else {
		
		// just use the format string
		ret = __rcall(rp, fmt);
	}

	return ret;
}

static struct s_table *_rcall_table;

/*****************************************************************************
 * rcall_hook
 *
 * Sets the rcall hook to be called when an rcall to function <func> is sent 
 * to this process at target index 0.
 */

int rcall_hook(const char *func, rcall_hook_t hook) {
	_rcall_table = s_table_set(_rcall_table, func, (void*) hook);

	return 0;
}

char *rcall_call(rp_t source, const char *args) {
	rcall_hook_t hook;
	char **argv;
	char *rets;
	int argc;

	argv = strparse(args, " ");
	if (!argv) return NULL;
	
	hook = (rcall_hook_t) s_table_get(_rcall_table, argv[0]);
	if (!hook) {
		for (argc = 0; argv[argc]; argc++) free(argv[argc]);
		free(argv);
		
		return NULL;
	}

	for (argc = 0; argv[argc]; argc++);

	rets = hook(source, argc, argv);

	for (argc = 0; argv[argc]; argc++) free(argv[argc]);
	free(argv);

	return rets;
}
