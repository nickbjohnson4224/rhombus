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

#include <vfsd.h>

struct vfs *vfs_root;
uint32_t m_vfs;

static char *path_preprocess(char *path) {
	char buffer[1000];
	size_t i, j;

	i = 0;
	j = 0;

	for (; path[i] == '/'; i++);

	for (; path[i]; i++, j++) {
		if (!(path[i] == '/' && path[i+1] == '/')) {
			buffer[j] = path[i];
		}
	}

	buffer[j] = '\0';

	strcpy(path, buffer);

	return path;
}

void vfs_call(uint32_t caller, struct packet *packet) {
	struct call *header;
	struct vfs *file;

	if (!packet) {
		send(PORT_REPLY, caller, NULL);
		return;
	}

	header = packet_getbuf(packet);

	if (!strncmp(header->name, "getdict", 16)) {
		path_preprocess(header->args);

		mutex_spin(&m_vfs);
		file = vfs_get(vfs_root, header->args);
		
		if (file && file->server) {
			header->size = sprintf(header->args, 
				"%d:%d", file->server, (uint32_t) file->inode);
		}
		else {
			header->name[0] = '!';
			header->size = 0;
		}
	}
	else {
		header->name[0] = '!';
		header->size = 0;
	}

	packet_setbuf(&packet, header->size + sizeof(struct call));
	send(PORT_REPLY, caller, packet);
	packet_free(packet);
}

void vfs_handle(uint32_t caller, struct packet *packet) {
	struct vfs_query *q;
	struct vfs *file;

	if (!packet) {
		packet = packet_alloc(0);
		send(PORT_ERROR, caller, packet);
		packet_free(packet);
		return;
	}

	q = packet_getbuf(packet);

	switch (q->command) {
	case VFS_CMD_FIND:
		path_preprocess(q->path0);

		mutex_spin(&m_vfs);
		file = vfs_get(vfs_root, q->path0);
		mutex_free(&m_vfs);

		if (file && file->server) {
			q->server = file->server;
			q->inode  = file->inode;
		}
		else {
			packet->data_length = 0;
		}
		break;
	case VFS_CMD_ADD:
		path_preprocess(q->path0);

		mutex_spin(&m_vfs);
		vfs_add(vfs_root, q->path0, q->server, q->inode);
		mutex_free(&m_vfs);
		break;
	case VFS_CMD_LIST:
		path_preprocess(q->path0);

		mutex_spin(&m_vfs);
		vfs_list(vfs_root, q->path0, q->path1);
		mutex_free(&m_vfs);
		break;
	}

	q->command = VFS_CMD_REPLY;
	send(PORT_REPLY, caller, packet);
	packet_free(packet);
}

int main() {

	when(PORT_QUERY, vfs_handle);
	when(PORT_CALL,  vfs_call);

	mutex_spin(&m_vfs);
	vfs_root = calloc(sizeof(struct vfs), 1);
	mutex_free(&m_vfs);

	printf("vfsd: ready\n");

	send(PORT_SYNC, getppid(), NULL);
	_done();

	return 0;
}
