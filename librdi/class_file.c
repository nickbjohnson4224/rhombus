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
#include <proc.h>
#include <page.h>
#include <ipc.h>

#include <rdi/core.h>
#include <rdi/io.h>

rdi_read_hook  rdi_global_read_hook;
rdi_write_hook rdi_global_write_hook;
rdi_mmap_hook  rdi_global_mmap_hook;
rdi_share_hook rdi_global_share_hook;

// XXX SEC - does not check read access
static void __rdi_read (struct msg *msg) {
	rdi_read_hook read_hook;
	struct robject *file;
	struct msg *reply;
	uint64_t offset;
	uint64_t source;
	uint32_t index;
	uint32_t size;

	if (msg->length != sizeof(uint64_t) + sizeof(uint32_t)) {
		/* message is of the wrong size */
		merror(msg);
		return;
	}

	index = RP_INDEX(msg->target);

	file = robject_get(index);
	if (!file) {
		/* there is no corresponding robject */
		merror(msg);
		return;
	}

	if (!rdi_check_access(file, msg->source, ACCS_READ)) {
		// access denied
		merror(msg);
		return;
	}

	if (!robject_check_type(file, "file")) {
		/* robject is not a file */
		merror(msg);
		return;
	}
	
	if (rdi_global_read_hook) {
		/* use global hook if defined */
		read_hook = rdi_global_read_hook;
	}
	else {
		/* default to file-specific hook */
		read_hook = (rdi_read_hook) robject_data(file, "read");
		if (!read_hook) {
			/* no hook; fail */
			merror(msg);
			return;
		}
	}

	/* read parameters */
	offset = ((uint64_t*) msg->data)[0];
	size   = ((uint32_t*) msg->data)[2];

	/* construct reply message containing buffer */
	reply = aalloc(sizeof(struct msg) + size, PAGESZ);
	reply->source = msg->target;
	reply->target = msg->source;
	reply->length = size;
	reply->port   = PORT_REPLY;
	reply->arch   = ARCH_NAT;

	source = msg->source;
	free(msg);

	reply->length = read_hook(file, source, reply->data, size, offset);

	msend(reply);
}

static void __rdi_write(struct msg *msg) {
	rdi_write_hook write_hook;
	struct robject *file;
	uint64_t offset;

	if (msg->length < sizeof(uint64_t)) {
		/* message is of the wrong size */
		merror(msg);
		return;
	}

	file = robject_get(RP_INDEX(msg->target));
	if (!file) {
		/* there is no corresponding robject */
		merror(msg);
		return;
	}

	if (!rdi_check_access(file, msg->source, ACCS_WRITE)) {
		// access denied
		merror(msg);
		return;
	}

	if (!robject_check_type(file, "file")) {
		/* robject is not a file */
		merror(msg);
		return;
	}
	
	if (rdi_global_write_hook) {
		/* use global hook if defined */
		write_hook = rdi_global_write_hook;
	}
	else {
		/* default to file-specific hook */
		write_hook = (rdi_write_hook) robject_data(file, "write");
		if (!write_hook) {
			/* no hook; fail */
			merror(msg);
			return;
		}
	}

	/* read parameter */
	offset = ((uint64_t*) msg->data)[0];

	((uint32_t*) msg->data)[0] = write_hook(file, msg->source, 
		&msg->data[sizeof(uint64_t)], msg->length - sizeof(uint64_t), offset);
	msg->length = sizeof(uint32_t);

	mreply(msg);
}

static void __rdi_share(struct msg *msg) {
	struct robject *file;
	off_t offset;
	uint8_t err;
	void *pages;

	if (msg->length < PAGESZ - sizeof(struct msg)) {
		merror(msg);
		return;
	}

	file = robject_get(RP_INDEX(msg->target));
	if (!file || !robject_check_type(file, "share")) {
		merror(msg);
		return;
	}

	if (!rdi_check_access(file, msg->source, ACCS_WRITE)) {
		// access denied
		merror(msg);
		return;
	}

	if (!rdi_global_share_hook) {
		merror(msg);
		return;
	}

	offset = ((uint64_t*) msg->data)[0];
	pages = aalloc(msg->length - PAGESZ + sizeof(struct msg), PAGESZ);
	page_self(&msg->data[PAGESZ - sizeof(struct msg)], pages, msg->length - PAGESZ + sizeof(struct msg));

	err = rdi_global_share_hook(file, msg->source, pages, msg->length - PAGESZ + sizeof(struct msg), offset);

	msg->data[0] = err;
	msg->length = 1;
	mreply(msg);
}

static void __rdi_sync(struct msg *msg) {
	struct robject *file;

	file = robject_get(RP_INDEX(msg->target));
	if (!file) {
		merror(msg);
		return;
	}

	if (!robject_check_type(file, "file")) {
		merror(msg);
		return;
	}
	
	if (!rdi_check_access(file, msg->source, ACCS_WRITE)) {
		// access denied
		merror(msg);
		return;
	}

	free(robject_call(file, msg->source, "sync"));

	merror(msg);
}

static void __rdi_reset(struct msg *msg) {
	struct robject *file;

	file = robject_get(RP_INDEX(msg->target));
	if (!file) {
		merror(msg);
		return;
	}

	if (!robject_check_type(file, "file")) {
		merror(msg);
		return;
	}

	if (!rdi_check_access(file, msg->source, ACCS_WRITE)) {
		// access denied
		merror(msg);
		return;
	}

	free(robject_call(file, msg->source, "reset"));

	merror(msg);
}

static char *_size(struct robject *r, rp_t src, int argc, char **argv) {
	off_t *size;

	if (!rdi_check_access(r, src, ACCS_READ)) {
		return strdup("! denied");
	}

	size = robject_data(r, "size");

	if (size) {
		return saprintf("%u:%u", (uint32_t) (*size >> 32), (uint32_t) *size);
	}
	else {
		return strdup("0");
	}
}

static char *_reset(struct robject *r, rp_t src, int argc, char **argv) {
	return strdup("! nosys");
}

struct robject *rdi_class_file;

void __rdi_class_file_setup() {
	
	rdi_class_file = robject_cons(0, rdi_class_core);

	robject_set_call(rdi_class_file, "size", _size);
	robject_set_call(rdi_class_file, "reset", _reset);

	robject_set_data(rdi_class_file, "type", (void*) "file");
	robject_set_data(rdi_class_file, "name", (void*) "RDI-class-file");

	when(PORT_READ,  __rdi_read);
	when(PORT_WRITE, __rdi_write);
	when(PORT_SYNC,  __rdi_sync);
	when(PORT_RESET, __rdi_reset);
	when(PORT_SHARE, __rdi_share);
}

struct robject *rdi_file_cons(uint32_t index, uint32_t access) {
	struct robject *r;

	r = robject_cons(index, rdi_class_file);
	rdi_set_access_default(r, access);

	return r;
}

void rdi_file_free(struct robject *r) {
	robject_free(r);
}
