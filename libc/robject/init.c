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

#include <robject.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <proc.h>
#include <ipc.h>
#include <abi.h>

static char *__type(struct robject *self, rp_t src, int argc, char **argv) {
	const char *type;
	char *parent_type;
	char *final_type;

	type = robject_data(self, "type");	
	parent_type = robject_call(self->parent, src, "type");

	if (parent_type) {
		if (type) {
			final_type = strvcat(type, " ", parent_type, NULL);
			free(parent_type);
		}
		else {
			final_type = parent_type;
		}

		return final_type;
	}
	else {
		if (type) {
			return strdup(type);
		}
		else {
			return NULL;
		}
	}
}

static char *__ping(struct robject *self, rp_t src, int argc, char **argv) {
	return strdup("T");
}

static char *__open(struct robject *self, rp_t src, int argc, char **argv) {
	return strdup("T");
}

static char *__name(struct robject *self, rp_t src, int argc, char **argv) {
	char *name;
	char *full;
	char *proc;

	name = robject_data(self, "name");

	if (!name) {
		if (self->index || self == robject_root) {
			name = saprintf("%u", self->index);
		}
		else {
			// XXX SEC - should probably scramble this pointer somehow
			name = saprintf("abstract-%u", (uint32_t) self);
		}
	}
	else {
		name = strdup(name);
	}

	proc = getname(getpid());
	full = strvcat("[", proc, ":", name, "]", NULL);
	free(proc);
	free(name);
	return full;
}

static char *__subscribe(struct robject *self, rp_t src, int argc, char **argv) {
	rp_t target;

	if (argc == 1) {
		robject_add_subscriber(self, src);
		return strdup("T");
	}
	
	if (argc == 2) {
		target = ator(argv[1]);
		
		if (RP_PID(target) == RP_PID(src)) {
			robject_add_subscriber(self, target);
			return strdup("T");
		}
		else {
			return strdup("!permit");
		}
	}

	return strdup("!arg");
}

static char *__unsubscribe(struct robject *self, rp_t src, int argc, char **argv) {
	rp_t target;

	if (argc == 1) {
		robject_del_subscriber(self, src);
		return strdup("T");
	}
	
	if (argc == 2) {
		target = ator(argv[1]);
		
		if (RP_PID(target) == RP_PID(src)) {
			robject_del_subscriber(self, target);
			return strdup("T");
		}
		else {
			return strdup("!permit");
		}
	}

	return strdup("!arg");
}

static void __rcall_handler(struct msg *msg) {
	struct robject *ro;
	struct msg *reply;
	char *rets;
	
	ro = robject_get(RP_INDEX(msg->target));

	if (!ro) {
		merror(msg);
		return;
	}

	rets = robject_call(ro, msg->source, (const char*) msg->data);
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

struct robject *robject_root;

struct robject *robject_class_basic;

void __robject_init(void) {

	// create basic class
	robject_class_basic = robject_cons(0, NULL);

	robject_set_data(robject_class_basic, "type", (void*) "basic");
	robject_set_data(robject_class_basic, "name", (void*) "RLIBC-class-basic");

	robject_set_call(robject_class_basic, "type", __type);
	robject_set_call(robject_class_basic, "ping", __ping);
	robject_set_call(robject_class_basic, "name", __name);
	robject_set_call(robject_class_basic, "open", __open);
	robject_set_call(robject_class_basic, "subscribe",   __subscribe);
	robject_set_call(robject_class_basic, "unsubscribe", __unsubscribe);

	// allocate root object
	robject_root = robject_cons(0, robject_class_basic);
	robject_set(0, robject_root);

	// set rcall and event handlers
	when(PORT_RCALL, __rcall_handler);
}
