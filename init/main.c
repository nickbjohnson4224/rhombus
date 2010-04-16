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

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
	int32_t pid;
	device_t nulldev;
	uint32_t mutex = 0;

	signal_policy(SIG_REPLY, POLICY_QUEUE);

	pid = fork();

	if (pid < 0) {
		terminal.init(nulldev);
		fire(-pid, SIG_REPLY, NULL);
		for(;;);
	}

	while (!signal_recvs(SIG_REPLY, pid));

	stdout = fsetup(pid, 0, "r");

	printf("Flux Operating System 0.4a\n");
	printf("Launching Terminal Driver...\n");
	
	printf("\n");
	printf("testing mutexen:\n");

	mutex_lock(&mutex);
	printf("mutex locked\n");
	printf("mutex state: %s\n", mutex_test(&mutex) ? "locked" : "unlocked");
	printf("mutex lock attempt: %s\n", mutex_lock(&mutex) ? "success" : "failure");

	mutex_free(&mutex);
	printf("mutex freed\n");
	printf("mutex state: %s\n", (mutex_test(&mutex) ? "locked" : "unlocked"));

	mutex_spin(&mutex);
	printf("mutex spinlocked\n");
	printf("mutex state: %s\n", mutex_test(&mutex) ? "locked" : "unlocked");

	for(;;);
}
