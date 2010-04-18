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

FILE *disk;

uint32_t mutex_i;
int i;
char *symbols = "AV.";

void thread_thingy(uint32_t caller, req_t *req) {
	int v;
	
	mutex_spin(&mutex_i);
	v = i;
	i++;
	mutex_free(&mutex_i);

	while (1) printf("%c", symbols[v % 3]);
}

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

	*file = fsetup(pid, 0, "r");
}

void printx(const char *str, size_t size) {
	size_t i;

	for (i = 0; i < size; i++) {
		if (!isprint(str[i])) {
			printf("\\%x", str[i]);
		}
		else {
			printf("%c", str[i]);
		}
	}
}

int main() {
	device_t nulldev;
	char buffer[8000];

	nulldev.type = -1;

	driver_start(&stdout, &terminal, nulldev);

	printf("Flux Operating System 0.4a\n");
	printf("Copyright 2010 Nick Johnson\n\n");

	printf("Launching Terminal Driver...\n");

	printf("Launching Keyboard Driver...\n");
	driver_start(&stdin, &keyboard, nulldev);

	printf("Launching ATA Driver...\n");
	driver_start(&disk, &ata, pci_findb(CLASS_STORAGE, PCI_CLASS, nulldev));

	printf("\nReading disk:\n");
	fread (buffer, sizeof(char), 8000, disk);
	fwrite(buffer, sizeof(char), 10, stdout);

	signal_policy(SIG_REPLY, POLICY_QUEUE);

	if (fork() < 0) {
		exec((uint8_t*) buffer, 8000);
	}

	signal_wait(SIG_REPLY, false);

	printf("!");

	for(;;);
}
