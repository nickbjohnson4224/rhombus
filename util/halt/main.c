/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <driver.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
	outb(0x64, 0xFE);

	return 0;
}
