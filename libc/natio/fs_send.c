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

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <natio.h>

/*****************************************************************************
 * fs_send
 *
 * Send the filesystem command <cmd> to the root directory <root>. The 
 * response is returned. <cmd> is freed. Returns NULL on error.
 */

struct mp_fs *fs_send(uint64_t root, struct mp_fs *cmd) {
	struct msg *reply;
	struct mp_fs *ret;
	struct mp_error *err;

	if (!root) {
		root = fs_root;
	}

	/* set up command */
	cmd->length   = sizeof(struct mp_fs);
	cmd->protocol = MP_PROT_FS;
	cmd->null0    = '\0';

	/* send and recieve */
	reply = rp_tsend(root, PORT_FS, (struct mp_basic*) cmd, 100);

	/* attempt to interpret as filesystem command */
	ret = fs_recv(reply);
	if (ret) {
		free(reply);
		return ret;
	}

	/* attempt to interpret as error */
	err = error_recv(reply);
	if (err) {
		free(err);
		free(reply);

		ret = malloc(sizeof(struct mp_fs));
		ret->op = FS_ERR;
		ret->v0 = ERR_NULL;
		return ret;
	}
	
	/* reject and return NULL */
	if (reply) {
		if (reply->packet) free(reply->packet);
		free(reply);
	}
	return NULL;
}
