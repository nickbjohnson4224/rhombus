/* 
 * Copyright 2009, 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <ipc.h>
#include <proc.h>
#include <exec.h>
#include <mmap.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "inc/tar.h"

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

	waits(PORT_SYNC, pid);

	if (file) {
		*file = fcons(pid, 0);
	}
}

int main() {
	extern void idle(void);
	extern void initrd_init(void);
	struct tar_file *boot_image, *file;
	char const **argv;

	/* Boot Image */
	boot_image = tar_parse((uint8_t*) BOOT_IMAGE);

	/* Terminal Driver */
	file = tar_find(boot_image, (char*) "term");
	if (!file) {
		for(;;);
	}
	daemon_start(&stdout, file->start, file->size, NULL);

	printf(splash);

	/* VFS Daemon */
	file = tar_find(boot_image, (char*) "vfsd");
	if (!file) {
		printf("critical error: no VFSd image found\n");
		for(;;);
	}
	daemon_start(&stdvfs, file->start, file->size, NULL);
	fadd("/vfsd", stdvfs->server, stdvfs->inode);
	fadd("/term", stdout->server, stdout->inode);

	/* Initrd */
	initrd_init();
	fadd("/initrd", getpid(), 0);

	/* Initrd over TARFS */
	argv = malloc(sizeof(char*) * 3);
	argv[0] = "tarfs";
	argv[1] = "/initrd";
	argv[2] = NULL;

	file = tar_find(boot_image, (char*) "tarfs");
	if (!file) {
		printf("critical error: no TARFS image found\n");
		for(;;);
	}
	daemon_start(NULL, file->start, file->size, argv);

	free(argv);

	/* Keyboard Driver */
	file = tar_find(boot_image, (char*) "kbd");
	if (!file) {
		printf("critical error: no keyboard driver found\n");
		for(;;);
	}
	daemon_start(&stdin, file->start, file->size, NULL);
	fadd("/kbd", stdin->server, stdin->inode);

	/* Flux Init Shell */
	file = tar_find(boot_image, (char*) "fish");
	if (!file) {
		printf("critical error: no init shell found\n");
		for(;;);
	}

	if (fork() < 0) {
		execiv(file->start, file->size, NULL);
	}

	waits(PORT_DEATH, 0);

	printf("INIT PANIC: system daemon died\n");

	for(;;);

	return 0;
}
