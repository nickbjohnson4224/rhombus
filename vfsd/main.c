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

struct vfsd_node {
	int type; /* 0 - device, 1 - file, 2 - directory */
	char name[100];
	
	uint32_t target;
	uint32_t inode;

	struct vfsd_node **parentv;
	size_t parents;

	struct vfsd_node **childv;
	size_t children;
} vfs;

static struct vfsd_node *vfsd_find_path(const char *path);
static struct vfsd_node *vfsd_find_index(uint32_t target, uint32_t resource);

int main() {

	printf("VFSd: ready\n");

	for(;;);
}
