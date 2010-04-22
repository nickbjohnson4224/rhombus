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

#include <vfsd.h>

struct vfs_node *vfs;
uint32_t m_vfs = 0;

int main() {
	struct vfs_node *node;
	const char **pathv;

	mutex_lock(&m_vfs);

	printf("VFSd: creating basic structure\n");

	vfs = vfs_node_new("vfs", TYPE_DIR);
	vfs_node_add(vfs, vfs_node_new("dev", TYPE_DIR));
	vfs_node_add(vfs, vfs_node_new("fs", TYPE_DIR));
	vfs_node_add(vfs_find("dev/"), vfs_node_new("stdout", TYPE_FILE));

	node = vfs_find("dev/stdout");
	if (node) printf("%s\n", node->name);

	printf("VFSd: ready\n");

	mutex_free(&m_vfs);

	fire(1, SIG_REPLY, NULL);

	for(;;);
}
