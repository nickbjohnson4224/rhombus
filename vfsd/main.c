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
#define QUERY_DEL	1
#define QUERY_FIND	2
#define QUERY_MNT	3
#define QUERY_UMNT	4

void vfs_add_handle(uint32_t caller, req_t *req) {
	struct vfs_query *query;
	struct vfs_node *node;

	if (!req_check(req)) {
		if (!req) req = ralloc();
		req->format = REQ_ERROR;
		tail(caller, SIG_REPLY, req);
		return;
	}

	query = (void*) req_getbuf(req);

	node = vfs_node_new(query->name, vfs_get_server(query->server), 
		query->inode, query->node_type);

	vfs_add(query->path, node);

	tail(caller, SIG_REPLY, req_cksum(req));
}

void vfs_find_handle(uint32_t caller, req_t *req) {
	struct vfs_query *query;
	struct vfs_node *node;
	struct vfs_inode inode;

	if (!req_check(req)) {
		if (!req) req = ralloc();
		req->format = REQ_ERROR;
		tail(caller, SIG_REPLY, req);
		return;
	}

	query = (void*) req_getbuf(req);

	node = vfs_find(query->path);
	
	inode.server = node->server->target;
	inode.inode  = node->inode;

	req_setbuf(req, STDOFF, sizeof(struct vfs_inode));
	req->format = REQ_WRITE;
	memcpy(req_getbuf(req), &inode, sizeof(struct vfs_inode));

	tail(caller, SIG_REPLY, req_cksum(req));
}

int main() {
	char **pathv;

	signal_register(SIG_CTRL, vfs_add_handle);
	signal_register(SIG_INFO, vfs_find_handle);

	printf("VFSd: ready\n");

	fire(1, SIG_REPLY, NULL);

	for(;;);
}
