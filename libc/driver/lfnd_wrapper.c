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
#include <mutex.h>
#include <proc.h>

/*****************************************************************************
 * lfnd_wrapper
 *
 * Performs the requested actions of a FS_LFND command.
 */

void lfnd_wrapper(struct fs_cmd *cmd, uint32_t inode) {
	uint64_t file;

	/* find pointer to file without following terminal links */
	file = lfs_find(inode, cmd->s0, true);

	if (file) {
		/* return file pointer on success */
		cmd->v0 = file;
	}
	else {
		/* return ERR_FILE on failure */
		cmd->op = FS_ERR;
		cmd->v0 = ERR_FILE;
	}
}
