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

#include <stdint.h>
#include <natio.h>

/*****************************************************************************
 * fs_send
 *
 * Send the filesystem command <cmd> to the root directory <root>. The 
 * response is saved back to <cmd> and returned. Returns NULL on error.
 */

struct fs_cmd *fs_send(uint64_t root, struct fs_cmd *cmd) {
	size_t length;

	if (!root) {
		root = fs_root;
	}

	cmd->null0 = '\0';
	length = sizeof(struct fs_cmd);
	length = ssend(root, cmd, cmd, length, 0, PORT_FS);
	
	if (length != sizeof(struct fs_cmd)) {
		return NULL;
	}
	else {
		return cmd;
	}
}
