/* 
 * Copyright 2009, 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/packet.h>
#include <flux/driver.h>
#include <flux/proc.h>
#include <flux/arch.h>
#include <flux/ipc.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main() {

	printf("pmdd: process metadata daemon version 0.4a\n");
	printf("pmdd: starting on pid %d\n", getpid());

	printf("pmdd: ready\n");

	send(PORT_SYNC, 1, NULL);
	_done();

	return 0;
}
