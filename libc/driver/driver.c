/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <driver.h>
#include <stdlib.h>
#include <proc.h>
#include <ipc.h>

/*****************************************************************************
 * active_driver
 *
 * The active driver, obviously. The functions in this structure are used to
 * handle filesystem and file requests.
 */

struct driver *active_driver = NULL;

/*****************************************************************************
 * lfs_wrapper
 *
 * Handles all filesystem requests.
 */

void lfs_wrapper(struct packet *packet, uint8_t port, uint32_t caller) {
	struct fs_cmd *cmd;
	struct fs_obj *fobj, *new_fobj;
	uint64_t file;

	if (!packet) {
		psend(PORT_REPLY, caller, NULL);
		return;
	}

	if (packet->data_length != sizeof(struct fs_cmd)) {
		pfree(packet);
		psend(PORT_REPLY, caller, NULL);
		return;
	}

	cmd = pgetbuf(packet);
	cmd->null0 = '\0';

	switch (cmd->op) {
	case FS_FIND:
		file = lfs_find(packet->target_inode, cmd->s0);
		
		if (file) {
			cmd->v0 = file;
		}
		else {
			cmd->op = FS_ERR;
		}
		break;
	case FS_CONS:
		if (!active_driver->cons) {
			cmd->op = FS_ERR;
			break;
		}

		fobj = lfs_lookup(packet->target_inode);

		if (fobj) {
			new_fobj = active_driver->cons(cmd->v0);
			if (new_fobj) {
				lfs_push(fobj, new_fobj, cmd->s0);
				cmd->v0   = getpid();
				cmd->v0 <<= 32;
				cmd->v0  |= new_fobj->inode;
			}
			else {
				cmd->op = FS_ERR;
			}
		}
		else {
			cmd->op = FS_ERR;
		}
		break;	
	case FS_MOVE:
		cmd->op = FS_ERR;
		break;
	case FS_REMV:
		fobj = lfs_lookup(packet->target_inode);

		if (fobj) {
			if (lfs_pull(fobj)) {
				cmd->op = FS_ERR;
			}
			else {
				if (active_driver->free) {
					active_driver->free(fobj);
				}
				else {
					free(fobj);
				}
			}
		}
		else {
			if (active_driver->free) {
				active_driver->free(fobj);
			}
			else {
				free(fobj);
			}
		}
		break;
	case FS_LINK:
		fobj = lfs_lookup(packet->target_inode);

		if (fobj) {
			fobj->link = cmd->v0;
		}
		else {
			cmd->op = FS_ERR;
		}
		break;
	case FS_LIST:
		fobj = lfs_lookup(packet->target_inode);

		if (fobj) {
			if (lfs_list(fobj, cmd->v0, cmd->s0, 4000)) {
				cmd->op = FS_ERR;
			}
		}
		else {
			cmd->op = FS_ERR;
		}
		break;
	case FS_SIZE:
		fobj = lfs_lookup(packet->target_inode);

		if (fobj) {
			if (active_driver->size) {
				cmd->v0 = active_driver->size(fobj);
			}
			else {
				cmd->v0 = fobj->size;
			}
		}
		else {
			cmd->op = FS_ERR;
		}
		break;
	case FS_TYPE:
		fobj = lfs_lookup(packet->target_inode);

		if (fobj) {
			cmd->v0 = fobj->type;
		}
		else {
			cmd->op = FS_ERR;
		}
		break;
	default:
		cmd->op = FS_ERR;
	}

	psend(PORT_REPLY, caller, packet);
	pfree(packet);
}

/*****************************************************************************
 * read_wrapper
 *
 * Handles and redirects read requests to the current active driver.
 */

void read_wrapper(struct packet *packet, uint8_t port, uint32_t caller) {
	struct fs_obj *file;
	
	if (!packet || !active_driver->read) {
		psend(PORT_REPLY, caller, NULL);
		return;
	}

	file = lfs_lookup(packet->target_inode);

	if (!file || (file->type != FOBJ_FILE)) {
		pfree(packet);
		psend(PORT_REPLY, caller, NULL);
		return;
	}
	
	packet->data_length = active_driver->read
		(file, pgetbuf(packet), packet->data_length, packet->offset);

	psend(PORT_REPLY, caller, packet);
	pfree(packet);
}

/*****************************************************************************
 * write_wrapper
 *
 * Handles and redirects write requests to the current active driver.
 */

void write_wrapper(struct packet *packet, uint8_t port, uint32_t caller) {
	struct fs_obj *file;

	if (!packet || !active_driver->write) {
		psend(PORT_REPLY, caller, NULL);
		return;
	}

	file = lfs_lookup(packet->target_inode);

	if (!file || (file->type != FOBJ_FILE)) {
		pfree(packet);
		psend(PORT_REPLY, caller, NULL);
		return;
	}

	packet->data_length = active_driver->write
		(file, pgetbuf(packet), packet->data_length, packet->offset);
	
	psend(PORT_REPLY, caller, packet);
	pfree(packet);
}

/*****************************************************************************
 * sync_wrapper
 *
 * Handles and redirects sync requests to the current active driver.
 */

void sync_wrapper(struct packet *packet, uint8_t port, uint32_t caller) {
	struct fs_obj *file;

	if (!packet || !active_driver->sync) {
		psend(PORT_REPLY, caller, NULL);
		return;
	}

	file = lfs_lookup(packet->target_inode);
	
	if (!file) {
		pfree(packet);
		psend(PORT_REPLY, caller, NULL);
		return;
	}

	active_driver->sync(file);
	
	psend(PORT_REPLY, caller, packet);
	pfree(packet);
}

/*****************************************************************************
 * reset_wrapper
 *
 * Handles and redirects reset requests to the current active driver.
 */

void reset_wrapper(struct packet *packet, uint8_t port, uint32_t caller) {
	struct fs_obj *file;

	if (!packet || !active_driver->reset) {
		psend(PORT_REPLY, caller, NULL);
		return;
	}

	file = lfs_lookup(packet->target_inode);
	
	if (!file || (file->type != FOBJ_FILE)) {
		pfree(packet);
		psend(PORT_REPLY, caller, NULL);
		return;
	}
	
	active_driver->reset(file);
	
	psend(PORT_REPLY, caller, packet);
	pfree(packet);
}

/*****************************************************************************
 * irq_wrapper
 *
 * Handles and redirects irqs to the current active driver.
 */

void irq_wrapper(struct packet *packet, uint8_t port, uint32_t caller) {
	
	if (caller != 0) {
		psend(PORT_REPLY, caller, NULL);
		return;
	}

	if (!active_driver->irq) {
		return;
	}

	active_driver->irq();

	if (packet) {
		pfree(packet);
	}
}

/*****************************************************************************
 * driver_init
 *
 * Initializes the driver <driver> on the current process, allowing read/write
 * and filesystem control messages.
 */

void driver_init(struct driver *driver, int argc, char **argv) {
	struct fs_obj *root;
	
	active_driver = driver;

	when(PORT_IRQ,   irq_wrapper);
	when(PORT_FS,    lfs_wrapper);
	when(PORT_READ,  read_wrapper);
	when(PORT_WRITE, write_wrapper);
	when(PORT_SYNC,  sync_wrapper);
	when(PORT_RESET, reset_wrapper);

	if (active_driver->init) {
		active_driver->init(argc, argv);
	}
	else {
		root = calloc(sizeof(struct fs_obj), 1);
		root->type = FOBJ_FILE;
		root->size = 0;
		root->inode = 0;
		lfs_root(root);
	}
}
