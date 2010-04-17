/* 
 * Copyright 2009, 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/signal.h>
#include <flux/request.h>
#include <flux/proc.h>
#include <flux/driver.h>

#include <driver/terminal.h>
#include <driver/keyboard.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static void driver_start(FILE **file, struct driver_interface *driver) {
	int32_t pid;
	device_t nulldev;

	signal_policy(SIG_REPLY, POLICY_QUEUE);

	pid = fork();

	if (pid < 0) {
		driver->init(nulldev);
		fire(-pid, SIG_REPLY, NULL);
		for(;;);
	}

	signal_waits(SIG_REPLY, pid, true);

	*file = fsetup(pid, 0, "r");
}

int main() {
	driver_start(&stdout, &terminal);

	printf("Flux Operating System 0.4a\n");
	printf("Copyright 2010 Nick Johnson\n\n");

	printf("Launching Terminal Driver...\n");

	printf("Launching Keyboard Driver...\n");
	driver_start(&stdin, &keyboard);

	for(;;);
}
