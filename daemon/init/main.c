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
Written by Nick Johnson\n\
\n";

void panic(const char *message) {
	printf("INIT PANIC: %s\n", message);
	for(;;);
}

void daemon_start(FILE **file, void *image, size_t image_size, char const **argv) {
	int32_t pid;

	pid = fork();

	if (pid < 0) {
		execiv(image, image_size, argv);
		for(;;);
	}

	pwaits(PORT_CHILD, pid);

	if (file) {
		*file = __fcons(pid, 0, NULL);
	}
}

int main() {
	extern void idle(void);
	extern void initrd_init(void);
	struct tar_file *boot_image, *file;
	char const **argv;
	FILE *temp, *init;

	init = __fcons(getpid(), 0, NULL);

	setenv("NAME", "unknown");
	vfroot(init);
	vffile("/", "dev", 1);

	lfs_init();

	when(PORT_VFS, lfs_event);

	/* Boot Image */
	boot_image = tar_parse((uint8_t*) BOOT_IMAGE);

	argv = malloc(sizeof(char*) * 3);
	argv[1] = NULL;
	
	/* Terminal Driver */
	argv[0] = "term";
	if (!(file = tar_find(boot_image, (char*) "term"))) panic("no term found");
	daemon_start(&stdout, file->start, file->size, argv);

	stderr = stdout;

	printf(splash);

	/* Initrd */
	initrd_init();
	vffile("/dev/", "initrd", 2);

	/* Initrd over TARFS */
	argv[0] = "tarfs";
	argv[1] = "/dev/initrd";
	argv[2] = NULL;

	if (!(file = tar_find(boot_image, (char*) "tarfs"))) panic("no tarfs found");
	daemon_start(&temp, file->start, file->size, argv);

	vflmnt("/", "bin", temp);
	setenv("PATH", "/bin");
	argv[1] = NULL;

	/* Keyboard Driver */
	argv[0] = "kbd";
	if (!(file = tar_find(boot_image, (char*) "kbd"))) panic("no kbd found");
	daemon_start(&stdin, file->start, file->size, argv);

	/* Time Driver */
	argv[0] = "time";
	if (!(file = tar_find(boot_image, (char*) "time"))) panic("no time found");
	daemon_start(&temp, file->start, file->size, argv);
	vflmnt("/dev/", "time", temp);

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
	
	pwaits(PORT_CHILD, 0);

	printf("INIT PANIC: system daemon died\n");
	for(;;);
	return 0;
}
