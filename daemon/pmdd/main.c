/* 
 * Copyright 2009, 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/driver.h>
#include <flux/proc.h>
#include <flux/ipc.h>
#include <flux/io.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main() {

	printf("pmdd: ready\n");

	send(PORT_SYNC, getppid(), NULL);
	_done();

	return 0;
}
