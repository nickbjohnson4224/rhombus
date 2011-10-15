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

#include <struct.h>
#include <stdlib.h>
#include <string.h>
#include <natio.h>
#include <proc.h>
#include <ipc.h>

#include <rhombus.h>

static struct s_table *event_table = NULL;

int event(rp_t rp, const char *value) {
	struct msg *msg;

	if (!value) value = "";

	msg = aalloc(sizeof(struct msg) + strlen(value) + 1, PAGESZ);
	if (!msg) return 1;
	msg->source = RP_CONS(getpid(), 0);
	msg->target = rp;
	msg->length = strlen(value) + 1;
	msg->port   = PORT_EVENT;
	msg->arch   = ARCH_NAT;
	strcpy((char*) msg->data, value);

	return msend(msg);
}

int event_subscribe(rp_t rp) {
	char *reply;

	reply = rcall(rp, "open %d", STAT_EVENT);

	if (!reply) return 1;
	
	free(reply);
	return 0;
} 

int event_unsubscribe(rp_t rp) {
	char *reply;

	reply = rcall(rp, "close %d", STAT_EVENT);

	if (!reply) return 1;

	free(reply);
	return 0;
}

static void __event_handler(struct msg *msg) {
	event_t hook;
	char **argv;
	int argc;

	argv = strparse((const char*) msg->data, " ");
	if (!argv) return;

	for (argc = 0; argv[argc]; argc++);

	hook = (event_t) (uintptr_t) s_table_get(event_table, argv[0]);

	if (hook) {
		hook(msg->source, argc, argv);
	}

	for (argc = 0; argv[argc]; argc++) free(argv[argc]);
	free(argv);

	free(msg);
}

int event_hook(const char *type, event_t hook) {
	
	event_table = s_table_set(event_table, type, (void*) (uintptr_t) hook);
	when(PORT_EVENT, __event_handler);

	return 0;
}
