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

/*****************************************************************************
 * event
 *
 * Asynchronous event protocol. Sends an 8-byte value to the given resource,
 * without waiting for a reply. Returns zero on success, nonzero on error.
 */

int event(uint64_t rp, const char *value) {
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

/******************************************************************************
 * eventl
 *
 * Sends an event to every resource on the event list <list>. Returns zero on 
 * success, nonzero on error.
 */

int eventl(struct event_list *list, const char *value) {
	
	while (list) {
		event(list->target, value);
		list = list->next;
	}

	return 0;
}

int event_register(uint64_t rp) {
	char *reply;

	reply = rcall(rp, "register");

	if (!reply) return 1;
	
	free(reply);
	return 0;
}

int event_deregister(uint64_t rp) {
	char *reply;

	reply = rcall(rp, "deregister");

	if (!reply) return 1;

	free(reply);
	return 0;
}

/*****************************************************************************
 * _event_map
 *
 * Structure mapping events to their respective handlers.
 *
 * Notes:
 * Currently, this is implemented as a linked list, which has terrible 
 * scalability.
 */

static struct _event_map {
	struct _event_map *next;

	event_t handler;
	char *key;
} *_event_map = NULL;

static void _event_handler(struct msg *msg);

int event_set(const char *event, event_t handler) {
	struct _event_map *node;

	if (!_event_map) {
		when(PORT_EVENT, _event_handler);
	}

	node = malloc(sizeof(struct _event_map));
	node->next = _event_map;
	node->handler = handler;
	node->key = strdup(event);

	_event_map = node;

	return 0;
}

event_t event_get(const char *event) {
	struct _event_map *node;

	// get handler
	for (node = _event_map; node; node = node->next) {
		if (node->key && !strcmp(node->key, event)) {
			return node->handler;
		}
	}

	return NULL;
}

static void _event_handler(struct msg *msg) {
	event_t handler;
	char **argv;
	char *args;
	int argc;

	// parse arguments
	args = (char*) msg->data;
	argv = strparse(args, " ");
	for (argc = 0; argv[argc]; argc++);

	handler = event_get(argv[0]);
	if (!handler) return;

	handler(msg->source, argc, argv);

	for (argc = 0; argv[argc]; argc++) {
		free(argv[argc]);
	}
	free(argv);
	free(msg);
}
