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

FILE *disk;

void driver_start(FILE **file, struct driver_interface *driver, device_t dev) {
	int32_t pid;

	signal_policy(SIG_REPLY, POLICY_QUEUE);

	pid = fork();

	if (pid < 0) {
		driver->init(dev);
		fire(-pid, SIG_REPLY, NULL);
		for(;;);
	}

	signal_waits(SIG_REPLY, pid, true);

	*file = fdopen(fdsetup(pid, 0), "r");
}

int main() {
	device_t nulldev;
	struct tar_file *boot_image;
	int i;
	struct file *f;

	/* File descriptors */
	stdin  = fdopen(0, "w");
	stdout = fdopen(1, "r");
	stderr = fdopen(2, "w");

	nulldev.type = -1;

	driver_start(&stdout, &terminal, nulldev);

	printf("Flux Operating System 0.4a\n");
	printf("Copyright 2010 Nick Johnson\n\n");

	printf("Reading Boot Image...\n");
	boot_image = tar_parse((uint8_t*) BOOT_IMAGE);

	for(;;);
}
