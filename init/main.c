/* 
 * Copyright 2009, 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/signal.h>
#include <flux/request.h>
#include <flux/proc.h>
#include <flux/driver.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <driver/terminal.h>
#include <driver/keyboard.h>
#include <driver/ata.h>
#include <driver/pci.h>

FILE *disk = NULL;
FILE *ping = NULL;

static void ping_write(uint32_t caller, req_t *req) {
	req->format = REQ_READ;
	tail(caller, SIG_REPLY, req_cksum(req));
}

static void ping_init(device_t dev) {
	sigregister(SIG_WRITE, ping_write);
}

static struct driver_interface ping_driver = {
	ping_init,
	NULL, 
	NULL, 
	0
};

static void launch_driver(FILE **hand, struct driver_interface *drv, device_t dev) {
	int32_t pid;
	
	sighold(SIG_REPLY);

	pid = fork();

	if (pid < 0) {
		sigfree(SIG_REPLY);
		drv->init(dev);
		fire(-pid, SIG_REPLY, NULL);
		block(true);
		for(;;);
	}
	sigpull(SIG_REPLY);

	*hand = fsetup(pid, 0, "r");

	sigfree(SIG_REPLY);
}

static void segfault(uint32_t source, struct request *req) {
	if (req) rfree(req);

	printf("Segmentation Fault\n");
	exit(1);
}

int main() {
	extern void shell(void);
	void *test;
	size_t i;

	device_t nulldev;
	nulldev.type = -1;

	sigregister(SSIG_FAULT, segfault);
	sigregister(SSIG_PAGE, 	segfault);

	launch_driver(&stdout, &terminal, nulldev);
	launch_driver(&stdin,  &keyboard, nulldev);

	printf("Flux 0.3a booting...\n");

	launch_driver(&disk, &ata, pci_findb(CLASS_STORAGE, PCI_CLASS, nulldev));

	launch_driver(&ping, &ping_driver, nulldev);

	test = malloc(4000000);

	for (i = 0; i < 16; i++) fwrite(test, 1, 4000000, ping);

	free(test);

	shell();

	for(;;);
	return 0;
}
