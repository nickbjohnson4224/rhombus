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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <natio.h>
#include <proc.h>

#include <rdi/core.h>

static char *_subscribe(struct robject *self, rp_t src, int argc, char **argv) {
	rp_t target;

	if (!rdi_check_access(self, src, ACCS_READ)) return strdup("! denied");

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
			return strdup("! denied");
		}
	}

	return strdup("! arg");
}

static char *_unsubscribe(struct robject *self, rp_t src, int argc, char **argv) {
	rp_t target;

	if (!rdi_check_access(self, src, ACCS_READ)) return strdup("! denied");

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
			return strdup("! denied");
		}
	}

	return strdup("! arg");
}

struct robject *rdi_class_event;

void __rdi_class_event_setup() {
	
	rdi_class_event = robject_cons(0, rdi_class_core);

	robject_set_call(rdi_class_event, "subscribe", _subscribe);
	robject_set_call(rdi_class_event, "unsubscribe", _unsubscribe);

	robject_set_data(rdi_class_event, "type", (void*) "event");
	robject_set_data(rdi_class_event, "name", (void*) "RDI-class-event");
}

struct robject *rdi_event_cons(uint32_t index, uint32_t access) {
	struct robject *r;

	r = robject_cons(index, rdi_class_event);
	rdi_set_access_default(r, access);

	return r;
}

void rdi_event_free(struct robject *r) {
	robject_free(r);
}
