/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
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

#include <ipc.h>
#include <proc.h>
#include <exec.h>
#include <mmap.h>
#include <dict.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <natio.h>

#include "inc/tar.h"

char logo[] = {
'\n',
'\t', ' ', ' ', ' ', ' ', 219, 219, 219, 219, '\n',
'\t', ' ', ' ', 219, 219, 219, 219, ' ', ' ', '\n',
'\t', ' ', ' ', ' ', ' ', 219, 219, ' ', ' ', '\n',
'\n', '\0'
};

const char *splash ="\
Flux Operating System 0.5a\n\
Copyright 2010 Nick Johnson\n\
\n";

void daemon_start(FILE **file, void *image, size_t image_size, char const **argv) {
	int32_t pid;

	pid = fork();

	if (pid < 0) {
		execiv(image, image_size, argv);
		for(;;);
	}

	pwaits(PORT_SYNC, pid);

	if (file) {
		*file = __fcons(pid, 0, NULL);
	}
}

int main() {
	extern void idle(void);
	extern void initrd_init(void);
	struct tar_file *boot_image, *file;
	char const **argv;
	size_t length;

	setenv("NAME", "unknown");

	/* Boot Image */
	boot_image = tar_parse((uint8_t*) BOOT_IMAGE);

	argv = malloc(sizeof(char*) * 3);
	argv[1] = NULL;
	
	/* Terminal Driver */
	argv[0] = "term";
	file = tar_find(boot_image, (char*) "term");
	if (!file) {
		for(;;);
	}
	daemon_start(&stdout, file->start, file->size, argv);
	stderr = stdout;

//	printf(logo);
	printf(splash);

	/* VFS Daemon */
	argv[0] = "vfsd";
	file = tar_find(boot_image, (char*) "vfsd");
	if (!file) {
		printf("critical error: no VFSd image found\n");
		for(;;);
	}
	daemon_start(&stdvfs, file->start, file->size, argv);
	fadd("/vfsd", stdvfs->server, stdvfs->inode);
	fadd("/term", stdout->server, stdout->inode);
	
	dlink("vfs:", "vfs:", stdvfs);

	/* Initrd */
	initrd_init();
	fadd("/initrd", getpid(), 0);

	/* Initrd over TARFS */
	argv[0] = "tarfs";
	argv[1] = "/initrd";
	argv[2] = NULL;
	file = tar_find(boot_image, (char*) "tarfs");
	if (!file) {
		printf("critical error: no TARFS image found\n");
		for(;;);
	}
	daemon_start(NULL, file->start, file->size, argv);
	argv[1] = NULL;

	/* Keyboard Driver */
	argv[0] = "kbd";
	file = tar_find(boot_image, (char*) "kbd");
	if (!file) {
		printf("critical error: no keyboard driver found\n");
		for(;;);
	}
	daemon_start(&stdin, file->start, file->size, argv);
	fadd("/kbd", stdin->server, stdin->inode);

	/* Flux Init Shell */
	argv[0] = "fish";
	file = tar_find(boot_image, (char*) "fish");
	if (!file) {
		printf("critical error: no init shell found\n");
		for(;;);
	}

	if (fork() < 0) {
		execiv(file->start, file->size, argv);
	}

	setenv("NAME", "init");
	
	pwaits(PORT_DEATH, 0);

	printf("INIT PANIC: system daemon died\n");

	for(;;);

	return 0;
}
