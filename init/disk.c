#include <stdio.h>
#include <stdlib.h>
#include <driver.h>
#include <signal.h>
#include <flux.h>

#include <driver/ata.h>
#include <driver/pci.h>

FILE *disk = NULL;

void disk_init(void) {
	int32_t pid;
	device_t dev;

	sighold(SIG_REPLY);

	dev.type = -1;
	dev = pci_findb(CLASS_STORAGE, PCI_CLASS, dev);

	pid = fork();
	if (pid < 0) {
		ata.init(dev);
		fire(-pid, SIG_REPLY, NULL);
/*		block(true); */
		for(;;);
	}

	disk = fsetup(pid, 0, "r");
	sigpull(SIG_REPLY);

	sigfree(SIG_REPLY);
}
