/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <rho/mutex.h>
#include <rho/proc.h>

#include <rdi/core.h>
#include <rdi/vfs.h>
#include <rdi/io.h>

size_t tmpfs_read(struct robject *self, rp_t source, uint8_t *buffer, size_t size, off_t offset) {
	uint8_t *file_data;
	off_t *file_size;

	mutex_spin(&self->driver_mutex);

	file_data = robject_data(self, "data");
	file_size = robject_data(self, "size");

	if (!file_data || !file_size) {
		mutex_free(&self->driver_mutex);
		return 0;
	}

	if (offset > *file_size) {
		mutex_free(&self->driver_mutex);
		return 0;
	}

	if (offset + size >= *file_size) {
		size = *file_size - offset;
	}

	memcpy(buffer, &file_data[offset], size);

	mutex_free(&self->driver_mutex);
	return size;
}

size_t tmpfs_write(struct robject *self, rp_t source, uint8_t *buffer, size_t size, off_t offset) {
	uint8_t *file_data;
	off_t _file_size = 0;
	off_t *file_size;

	mutex_spin(&self->driver_mutex);

	file_data = robject_data(self, "data");
	file_size = robject_data(self, "size");

	if (!file_size) {
		file_size = &_file_size;
	}

	if (offset + size >= *file_size) {
		file_data = realloc(file_data, offset + size);
		robject_set_data(self, "data", file_data);
		if (file_size == &_file_size) {
			file_size = malloc(sizeof(off_t));
		}
		*file_size = offset + size;
		robject_set_data(self, "size", file_size);
	}

	memcpy(&file_data[offset], buffer, size);

	mutex_free(&self->driver_mutex);
	return size;
}

struct robject *tmpfs_file_cons(rp_t source, int argc, char **argv) {
	return rdi_file_cons(robject_new_index(), ACCS_READ | ACCS_WRITE);
}

struct robject *tmpfs_dir_cons(rp_t source, int argc, char **argv) {
	return rdi_dir_cons(robject_new_index(), ACCS_READ | ACCS_WRITE);
}

struct robject *tmpfs_link_cons(rp_t source, int argc, char **argv) {
	return rdi_link_cons(robject_new_index(), ACCS_READ | ACCS_WRITE, NULL);
}

char *tmpfs_reset(struct robject *self, rp_t source, int argc, char **argv) {
	uint8_t *file_data;
	off_t *file_size;

	mutex_spin(&self->driver_mutex);

	file_data = robject_data(self, "data");
	file_size = robject_data(self, "size");

	free(file_data);
	free(file_size);

	robject_set_data(self, "data", NULL);
	robject_set_data(self, "size", NULL);

	mutex_free(&self->driver_mutex);

	return strdup("T");
}

int main(int argc, char **argv) {
	struct robject *root;

	rdi_init();

	// create root directory
	root = rdi_dir_cons(robject_new_index(), ACCS_READ | ACCS_WRITE);

	// set interface functions
	robject_set_call(rdi_class_file, "reset", tmpfs_reset, STAT_WRITER);
	rdi_global_read_hook  = tmpfs_read;
	rdi_global_write_hook = tmpfs_write;
	rdi_global_cons_file_hook = tmpfs_file_cons;
	rdi_global_cons_dir_hook  = tmpfs_dir_cons;
	rdi_global_cons_link_hook = tmpfs_link_cons;

	// daemonize
	msendb(RP_CONS(getppid(), 0), ACTION_CHILD);
	_done();

	return 0;
}
