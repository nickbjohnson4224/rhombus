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

char *rcall(uint64_t rp, const char *fmt, ...) {
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

static char *__rcalls(rp_t rp, rp_t src, const char *args) {
	struct msg *msg;
	size_t length;
	char *rets;

	length = strlen(args) + 1;

	msg = aalloc(sizeof(struct msg) + length, PAGESZ);
	if (!msg) return NULL;
	msg->source = src;
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

char *rcalls(rp_t rp, rp_t src, const char *fmt, ...) {
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
		ret = __rcalls(rp, src, args);

		// free argument string
		free(args);
	}
	else {
		
		// just use the format string
		ret = __rcalls(rp, src, fmt);
	}

	return ret;
}
