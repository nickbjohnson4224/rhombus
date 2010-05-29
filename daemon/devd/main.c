/* 
 * Copyright 2009, 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/ipc.h>
#include <flux/proc.h>
#include <flux/driver.h>
#include <flux/io.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main() {

	printf("devd: ready\n");

	send(PORT_SYNC, 1, NULL);
	_done();

	return 0;
}
