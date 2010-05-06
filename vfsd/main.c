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

#define QUERY_ADD	0
#define QUERY_REM	1
#define QUERY_FIND	2
#define QUERY_LINK  3

struct vfs *vfs_root;
uint32_t m_vfs;

void vfs_handle(uint32_t caller, req_t *req) {
	struct vfs_query *q;
	struct vfs *file;

	printf("VFS: handling query...\n");

	if (!req_check(req)) {
		if (!req) req = ralloc();
		req->format = REQ_ERROR;
		tail(caller, SIG_REPLY, req);
		return;
	}

	q = (void*) req_getbuf(req);

	switch (q->command) {
	case VFS_CMD_FIND:
		printf("VFS: find %s\n", q->path0);
		file = vfs_get(vfs_root, q->path0);
		if (file) {
			q->server = file->server;
			q->inode  = file->inode;
		}
		break;
	}

	q->command = VFS_CMD_REPLY;
	tail(caller, SIG_REPLY, req_cksum(req));
}

int main() {

	vfs_root = calloc(sizeof(struct vfs), 1);

	printf("VFSd: ready\n");

	fire(1, SIG_REPLY, NULL);

	for(;;);
}
