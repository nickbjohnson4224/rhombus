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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <rho/proc.h>
#include <rho/ipc.h>
#include <rho/abi.h>

#include <rdi/robject.h>
#include <rdi/core.h>

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

static char *__find(struct robject *self, rp_t src, int argc, char **argv) {
	return rtoa(RP_CONS(getpid(), self->index));
}

static char *__open(struct robject *self, rp_t src, int argc, char **argv) {
	int status;

	if (argc == 2) {
		status = atoi(argv[1]);
	}
	else if (argc == 1) {
		status = STAT_OPEN;
	}
	else {
		return errorstr(EINVAL);
	}

	if (!robject_check_access(self, src, status &~ STAT_OPEN)) {
		return errorstr(EACCES);
	}

	if (status) {
		rtab_open(RP_PID(src), RP_CONS(getpid(), self->index));
	}

	robject_open(self, src, status);
	
	return strdup("T");
}

static char *__close(struct robject *self, rp_t src, int argc, char **argv) {

	if (argc == 1) {
		robject_close(self, src);
		return strdup("T");
	}
	else {
		return errorstr(EINVAL);
	}
}

static char *__stat(struct robject *self, rp_t src, int argc, char **argv) {
	return saprintf("%d", robject_stat(self, src));
}

static char *__name(struct robject *self, rp_t src, int argc, char **argv) {
	char *name;
	char *full;
	char *proc;

	name = robject_data(self, "name");

	if (!name) {
		if (self->index) {
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

static char *_get_access(struct robject *r, rp_t src, int argc, char **argv) {
	uint32_t bitmap;
	uint32_t user;

	if (argc == 1) {
		user = getuser(RP_PID(src));
	}
	else if (argc == 2) {
		user = atoi(argv[1]);
	}
	else {
		return errorstr(EINVAL);
	}

	bitmap = robject_get_access(r, user);

	return saprintf("%X", bitmap);
}

static char *_set_access(struct robject *r, rp_t src, int argc, char **argv) {
	uint32_t bitmap;
	uint32_t user;

	if (argc == 2) {
		user = getuser(RP_PID(src));
		sscanf(argv[1], "%X", &bitmap);
	}
	else if (argc == 3) {
		user = atoi(argv[1]);
		sscanf(argv[2], "%X", &bitmap);
	}
	else {
		return errorstr(EINVAL);
	}

	robject_set_access(r, user, bitmap);

	return strdup("T");
}

rdi_cons_hook rdi_global_cons_file_hook;
rdi_cons_hook rdi_global_cons_dir_hook;
rdi_cons_hook rdi_global_cons_link_hook;

static char *_cons(rp_t src, int argc, char **argv) {
	struct robject *new_robject = NULL;
	char *type;

	if (argc >= 2) {
		type = argv[1];

		if (!strcmp(type, "file")) {
			if (rdi_global_cons_file_hook) {
				new_robject = rdi_global_cons_file_hook(src, argc, argv);
			}
		}
		else if (!strcmp(type, "link")) {
			if (rdi_global_cons_link_hook) {
				new_robject = rdi_global_cons_link_hook(src, argc, argv);
			}
		}
		else if (!strcmp(type, "dir")) {
			if (rdi_global_cons_dir_hook) {
				new_robject = rdi_global_cons_dir_hook(src, argc, argv);
			}
		}

		if (new_robject) {
			rtab_open(src, RP_CONS(getpid(), new_robject->index));
			robject_open(new_robject, src, STAT_OPEN | robject_stat(new_robject, src));
			return rtoa(RP_CONS(getpid(), new_robject->index));
		}
	}

	return errorstr(EINVAL);
}

static void __rcall_handler(struct msg *msg) {
	struct robject *ro;
	struct msg *reply;
	char *rets;
	
	if (RP_INDEX(msg->target) != 0) {
		ro = robject_get(RP_INDEX(msg->target));

		if (!ro) {
			merror(msg);
			return;
		}

		rets = robject_call(ro, msg->source, (const char*) msg->data);
		if (!rets) rets = strdup("");
	}
	else {	
		rets = rcall_call(msg->source, (const char*) msg->data);
		if (!rets) rets = strdup("");
	}

	reply = aalloc(sizeof(struct msg) + strlen(rets) + 1, PAGESZ);
	reply->source = msg->target;
	reply->target = msg->source;
	reply->length = strlen(rets) + 1;
	reply->action = ACTION_REPLY;
	reply->arch   = ARCH_NAT;
	strcpy((char*) reply->data, rets);
	free(rets);

	free(msg);
	msend(reply);
}

static void __close_handler(struct msg *msg) {
	struct robject *ro;

	ro = robject_get(RP_INDEX(msg->target));
	if (!ro) return;

	free(robject_call(ro, msg->source, "close"));
	free(msg);
}

struct robject *rdi_class_core;

void __rdi_class_core_setup(void) {

	// create core class
	rdi_class_core = robject_cons(0, NULL);

	robject_set_data(rdi_class_core, "type", (void*) "event basic");
	robject_set_data(rdi_class_core, "name", (void*) "RDI-class-core");

	robject_set_call(rdi_class_core, "type", __type, 0);
	robject_set_call(rdi_class_core, "ping", __ping, 0);
	robject_set_call(rdi_class_core, "name", __name, 0);
	robject_set_call(rdi_class_core, "open", __open, 0);
	robject_set_call(rdi_class_core, "close", __close, STAT_OPEN);
	robject_set_call(rdi_class_core, "stat", __stat, 0);
	robject_set_call(rdi_class_core, "find", __find, 0);
	robject_set_call(rdi_class_core, "get-access", _get_access, 0);
	robject_set_call(rdi_class_core, "set-access", _set_access, STAT_ADMIN);

	// set rcall and close handlers
	when(ACTION_RCALL, __rcall_handler);
	when(ACTION_CLOSE, __close_handler);

	// set constructor
	rcall_hook("cons", _cons);
}

struct robject *rdi_core_cons(uint32_t index, uint32_t access) {
	struct robject *r;

	r = robject_cons(index, rdi_class_core);
	robject_set_default_access(r, access);

	return r;
}

void rdi_core_free(struct robject *r) {
	robject_free(r);
}
