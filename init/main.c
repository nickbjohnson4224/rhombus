/* 
 * Copyright 2009, 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/signal.h>
#include <flux/request.h>
#include <flux/proc.h>
#include <flux/driver.h>
#include <flux/exec.h>
#include <flux/vfs.h>
#include <flux/io.h>

#include <driver/terminal.h>
#include <driver/keyboard.h>
#include <driver/ata.h>
#include <driver/pci.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "inc/tar.h"

void driver_start(int fd, struct driver_interface *driver, device_t dev) {
	int32_t pid;

	signal_policy(SIG_REPLY, POLICY_QUEUE);

	pid = fork();

	if (pid < 0) {
		driver->init(dev);
		fire(-pid, SIG_REPLY, NULL);
		for(;;);
	}

	signal_waits(SIG_REPLY, pid, true);

	fdset(fd, pid, 0);
}

void daemon_start(int fd, void *image, size_t image_size) {
	int32_t pid;

	signal_policy(SIG_REPLY, POLICY_QUEUE);

	pid = fork();

	if (pid < 0) {
		exec(image, image_size);
		for(;;);
	}

	signal_waits(SIG_REPLY, pid, true);

	fdset(fd, pid, 0);
}

int main() {
	device_t nulldev;
	struct tar_file *boot_image, *file;
	struct file *f;
	int i;

	nulldev.type = -1;

	/* Boot Image */
	boot_image = tar_parse((uint8_t*) BOOT_IMAGE);

	/* Terminal Driver */
	driver_start(FD_STDOUT, &terminal, nulldev);

	printf("Flux Operating System 0.4a\n");
	printf("Copyright 2010 Nick Johnson\n\n");

	/* VFS Daemon */
	file = tar_find(boot_image, (char*) "vfsd");
	if (!file) {
		printf("critical error: no VFSd image found\n");
		for(;;);
	}
	daemon_start(FD_STDVFS, file->start, file->size);

	i = find("cake");
	f = fdget(i);
	printf("cake: %d %d\n", f->target, f->resource);

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

	for(;;);
}
