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
#include <driver.h>
#include <mutex.h>
#include <stdio.h>
#include <natio.h>
#include <proc.h>
#include <vfs.h>

#define PIPESIZE 1024

struct pipe {
	uint8_t buffer[PIPESIZE];
	int tail;
	int head;
	bool mutex;
	bool closed;
};

static uint8_t pipe_readbyte(struct pipe *pipe) {
	uint8_t byte;
	
	mutex_spin(&pipe->mutex);

	while (pipe->tail == pipe->head) {
		mutex_free(&pipe->mutex);
		sleep();
		mutex_spin(&pipe->mutex);
	}

	byte = pipe->buffer[pipe->tail];
	pipe->tail = (pipe->tail + 1) % PIPESIZE;

	mutex_free(&pipe->mutex);

	return byte;
}

static int pipe_writebyte(struct pipe *pipe, uint8_t byte) {
	
	mutex_spin(&pipe->mutex);

	while (pipe->head == (pipe->tail - 1) % PIPESIZE) {
		mutex_free(&pipe->mutex);
		sleep();
		mutex_spin(&pipe->mutex);
	}

	pipe->buffer[pipe->head] = byte;
	pipe->head = (pipe->head + 1) % PIPESIZE;

	mutex_free(&pipe->mutex);

	return 0;
}

// 
// Q. what do you call a multithreaded pipe driver?
//
// A. parallelpiped!
//

char *pipe_rcall_pipe(uint64_t source, uint32_t index, int argc, char **argv) {
	struct resource *pipe_head;
	struct resource *pipe_tail;
	static uint32_t index_top = 2;
	struct pipe *pipe;

	pipe = calloc(sizeof(struct pipe), 1);
	
	pipe_head = resource_cons(FS_TYPE_FILE, PERM_READ);
	pipe_tail = resource_cons(FS_TYPE_FILE, PERM_WRITE);
	pipe_head->type |= FS_TYPE_CHAR;
	pipe_tail->type |= FS_TYPE_CHAR;

	pipe_head->data = pipe;
	pipe_tail->data = pipe;

	index_set(index_top,     pipe_head);
	index_set(index_top + 1, pipe_tail);
	index_top += 2;

	return saprintf("%r %r", RP_CONS(getpid(), pipe_head->index), RP_CONS(getpid(), pipe_tail->index));
}

size_t pipe_read(uint64_t source, uint32_t index, uint8_t *buffer, size_t size, uint64_t offset) {
	struct resource *r = index_get(index);
	struct pipe *pipe = r->data;
	size_t i;

	if (!pipe) {
		return 0;
	}
	
	for (i = 0; i < size; i++) {
		if (pipe->closed && pipe->head == (pipe->tail - 1) % PIPESIZE) {
			break;
		}

		buffer[i] = pipe_readbyte(pipe);
	}

	return i;
}

size_t pipe_write(uint64_t source, uint32_t index, uint8_t *buffer, size_t size, uint64_t offset) {
	struct resource *r = index_get(index);
	struct pipe *pipe = r->data;
	size_t i;

	if (!pipe) {
		return 0;
	}

	for (i = 0; i < size; i++) {
		pipe_writebyte(pipe, buffer[i]);
	}

	return size;
}

int pipe_close(uint64_t source, struct resource *r) {
	struct pipe *pipe = r->data;

	if (!pipe) {
		return 1;
	}

	pipe->closed = true;

	return 0;
}
	
/*****************************************************************************
 * pipe - pipe driver
 *
 */

int main(int argc, char **argv) {
	index_set(0, resource_cons(FS_TYPE_FILE, PERM_READ));

	rcall_set("pipe", pipe_rcall_pipe);
	di_wrap_read (pipe_read);
	di_wrap_write(pipe_write);
	vfs_set_close(pipe_close);
	vfs_init();

	msendb(RP_CONS(getppid(), 0), PORT_CHILD);
	setppid(1);
	done();

	return 0;
}
