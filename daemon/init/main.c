/* 
 * Copyright 2009, 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/ipc.h>
#include <flux/proc.h>
#include <flux/exec.h>
#include <flux/mmap.h>
#include <flux/io.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "inc/tar.h"

const char *splash ="\
Flux Operating System 0.4a\n\
Copyright 2010 Nick Johnson\n\
\n";

void daemon_start(int fd, void *image, size_t image_size, char const **argv) {
	int32_t pid;

	pid = fork();

	if (pid < 0) {
		execiv(image, image_size, argv);
		for(;;);
	}

	waits(PORT_SYNC, pid);

	if (fd != -1) {
		fdset(fd, pid, 0);
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
	daemon_start(FD_STDOUT, file->start, file->size, NULL);

	printf(splash);

	/* VFS Daemon */
	file = tar_find(boot_image, (char*) "vfsd");
	if (!file) {
		printf("critical error: no VFSd image found\n");
		for(;;);
	}
	daemon_start(FD_STDVFS, file->start, file->size, NULL);
	fadd("/vfsd", fdget(FD_STDVFS)->server, fdget(FD_STDVFS)->inode);
	fadd("/term", fdget(FD_STDOUT)->server, fdget(FD_STDOUT)->inode);

//	/* Device Daemon */
//	file = tar_find(boot_image, (char*) "dict");
//	if (!file) {
//		printf("critical error: no DEVd image found\n");
//		for(;;);
//	}
//	daemon_start(FD_STDDEV, file->start, file->size, NULL);
//	fadd("/devd", fdget(FD_STDDEV)->server, fdget(FD_STDDEV)->inode);

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
	daemon_start(-1, file->start, file->size, argv);

	free(argv);

//	/* Process Metadata Daemon */
//	file = tar_find(boot_image, (char*) "pmdd");
//	if (!file) {
//		printf("critical error: no PMDd image found\n");
//		for(;;);
//	}
//	daemon_start(FD_STDPMD, file->start, file->size, NULL);
//	fadd("/pmdd", fdget(FD_STDPMD)->server, fdget(FD_STDPMD)->inode);

	/* Keyboard Driver */
	file = tar_find(boot_image, (char*) "kbd");
	if (!file) {
		printf("critical error: no keyboard driver found\n");
		for(;;);
	}
	daemon_start(FD_STDIN, file->start, file->size, NULL);
	fadd("/kbd", fdget(FD_STDIN)->server, fdget(FD_STDIN)->inode);

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
