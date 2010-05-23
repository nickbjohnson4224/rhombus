/* 
 * Copyright 2009, 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/ipc.h>
#include <flux/packet.h>
#include <flux/proc.h>
#include <flux/driver.h>
#include <flux/exec.h>
#include <flux/vfs.h>
#include <flux/io.h>
#include <flux/heap.h>

#include <driver/pci.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "inc/tar.h"

const char *splash ="\
Flux Operating System 0.4a\n\
Copyright 2010 Nick Johnson\n\
\n";

void daemon_start(int fd, void *image, size_t image_size) {
	int32_t pid;

	pid = fork();

	if (pid < 0) {
		exec(image, image_size);
		for(;;);
	}

	waits(PORT_SYNC, pid);

	fdset(fd, pid, 0);
}

int main() {
	extern void idle(void);
	struct tar_file *boot_image, *file;
	struct file *f;
	int i;

	/* Boot Image */
	boot_image = tar_parse((uint8_t*) BOOT_IMAGE);

	/* Terminal Driver */
	file = tar_find(boot_image, (char*) "terminal");
	if (!file) {
		for(;;);
	}
	daemon_start(FD_STDOUT, file->start, file->size);

	printf(splash);

	/* VFS Daemon */
	file = tar_find(boot_image, (char*) "vfsd");
	if (!file) {
		printf("critical error: no VFSd image found\n");
		for(;;);
	}
	daemon_start(FD_STDVFS, file->start, file->size);

	/* Device Daemon */
	file = tar_find(boot_image, (char*) "devd");
	if (!file) {
		printf("critical error: no DEVd image found\n");
		for(;;);
	}
	daemon_start(FD_STDDEV, file->start, file->size);

	/* Process Metadata Daemon */
	file = tar_find(boot_image, (char*) "pmdd");
	if (!file) {
		printf("critical error: no PMDd image found\n");
		for(;;);
	}
	daemon_start(FD_STDPMD, file->start, file->size);

	/* Keyboard Driver */
	file = tar_find(boot_image, (char*) "keyboard");
	if (!file) {
		printf("critical error: no keyboard driver found\n");
		for(;;);
	}
	daemon_start(FD_STDIN, file->start, file->size);

	idle();

	return 0;
}
