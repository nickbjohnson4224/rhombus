/* Copyright 2009, 2010 Nick Johnson */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <flux/flux.h>

#include <driver/terminal.h>
#include <driver/keyboard.h>
#include <driver/ata.h>
#include <driver/pci.h>

FILE *disk = NULL;

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

	device_t nulldev;
	nulldev.type = -1;

	sigregister(SSIG_FAULT, segfault);
	sigregister(SSIG_PAGE, 	segfault);

	launch_driver(&stdout, &terminal, nulldev);
	launch_driver(&stdin,  &keyboard, nulldev);

	printf("Flux 0.2a booting...\n");

	launch_driver(&disk,   &ata, pci_findb(CLASS_STORAGE, PCI_CLASS, nulldev));

	shell();

	for(;;);
	return 0;
}
