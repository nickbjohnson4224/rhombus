/* 
 * Copyright 2009, 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/signal.h>
#include <flux/request.h>
#include <flux/proc.h>
#include <flux/driver.h>
#include <flux/vfs.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <vfsd.h>

struct vfs *vfs_root;
uint32_t m_vfs;

void vfs_handle(uint32_t caller, req_t *req) {
	struct vfs_query *q;
	struct vfs *file;

	if (!req_check(req)) {
		if (!req) req = ralloc();
		req->format = REQ_ERROR;
		tail(caller, SIG_REPLY, req);
		return;
	}

	q = (void*) req_getbuf(req);

	switch (q->command) {
	case VFS_CMD_FIND:
		mutex_spin(&m_vfs);
		file = vfs_get(vfs_root, q->path0);
		mutex_free(&m_vfs);

		if (file) {
			q->server = file->server;
			q->inode  = file->inode;
		}
		break;
	case VFS_CMD_ADD:
		mutex_spin(&m_vfs);
		vfs_add(vfs_root, q->path0, q->server, q->inode);
		mutex_free(&m_vfs);
		break;
	}

	q->command = VFS_CMD_REPLY;
	tail(caller, SIG_REPLY, req_cksum(req));
}

int main() {
	signal_register(SIG_QUERY, vfs_handle);
	signal_policy(SIG_QUERY, POLICY_EVENT);

	printf("VFSd: starting\n");

	mutex_spin(&m_vfs);
	vfs_root = calloc(sizeof(struct vfs), 1);

	vfs_add(vfs_root, "cake", 42, 42);
	mutex_free(&m_vfs);

	printf("VFSd: ready\n");

	fire(1, SIG_REPLY, NULL);

	for(;;);
}
