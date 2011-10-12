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
#include <mutex.h>
#include <proc.h>

#include <rdi/core.h>
#include <rdi/vfs.h>
#include <rdi/arch.h>
#include <rdi/io.h>

#define ERR_EOF   0x100
#define ERR_EMPTY 0x101

struct pipe_node {
	struct pipe_node *next;
	int datum;
};

struct pipe {
	bool mutex;
	struct pipe_node *front;
	struct pipe_node *back;
};

static struct pipe *pipe_new(void) {
	struct pipe *pipe;

	pipe = malloc(sizeof(struct pipe));
	pipe->mutex = false;
	pipe->front = NULL;
	pipe->back  = NULL;

	return pipe;
}

static int pipe_getc(struct pipe *pipe) {
	struct pipe_node *node;
	int datum;

	mutex_spin(&pipe->mutex);
	if (pipe->front) {
		datum = pipe->front->datum;
		node = pipe->front;
		pipe->front = node->next;
		if (pipe->back == node) {
			pipe->back = NULL;
		}
		free(node);
	}
	else {
		datum = ERR_EMPTY;
	}
	mutex_free(&pipe->mutex);

	return datum;
}

static void pipe_putc(struct pipe *pipe, int datum) {
	struct pipe_node *node;
	
	node = malloc(sizeof(struct pipe_node));
	node->datum = datum;
	node->next = NULL;

	mutex_spin(&pipe->mutex);
	if (!pipe->back) {
		pipe->back = node;
		pipe->front = node;
	}
	else {
		pipe->back->next = node;
		pipe->back = node;
	}
	mutex_free(&pipe->mutex);
}

size_t pipe_read(struct robject *self, rp_t source, uint8_t *buffer, size_t size, off_t offset) {
	struct pipe *pipe;
	int datum;
	size_t i;

	pipe = robject_data(self, "pipe");

	for (i = 0; i < size; i++) {
		datum = pipe_getc(pipe);

		while (datum == ERR_EMPTY) {
			if (robject_get_refc(self) <= 1) {
				datum = ERR_EOF;
				break;
			}
			sleep();
			datum = pipe_getc(pipe);
		}

		if (datum == ERR_EOF) {
			size = i;
			break;
		}

		buffer[i] = (uint8_t) datum;
	}

	return size;
}

size_t pipe_write(struct robject *self, rp_t source, uint8_t *buffer, size_t size, off_t offset) {
	struct pipe *pipe;
	size_t i;

	pipe = robject_data(self, "pipe");
	
	for (i = 0; i < size; i++) {
		pipe_putc(pipe, buffer[i]);
	}

	return size;
}

char *pipe_cons(struct robject *self, rp_t src, int argc, char **argv) {
	struct pipe *pipe;
	struct robject *new_r = NULL;
	char *type;

	if (argc == 2) {
		type = argv[1];

		if (!strcmp(type, "file")) {
			new_r = rdi_file_cons(robject_new_index(), ACCS_READ | ACCS_WRITE);
			pipe = pipe_new();
			robject_set_data(new_r, "pipe", pipe);

			return rtoa(RP_CONS(getpid(), new_r->index));
		}

		return strdup("! type");
	}

	return strdup("! arg");
}

int main(int argc, char **argv) {
	struct robject *root;

	rdi_init();

	// create device file
	root = rdi_core_cons(0, ACCS_READ | ACCS_WRITE);
	robject_set(0, root);
	robject_root = root;

	// set interface functions
	robject_set_call(rdi_class_core, "cons", pipe_cons);
	rdi_global_write_hook = pipe_write;
	rdi_global_read_hook  = pipe_read;

	// daemonize
	msendb(RP_CONS(getppid(), 0), PORT_CHILD);
	_done();

	return 0;
}
