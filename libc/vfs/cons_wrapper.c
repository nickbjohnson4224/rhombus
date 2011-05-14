/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and distribute this software for any
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
#include <stdlib.h>
#include <stdio.h>
#include <mutex.h>
#include <proc.h>
#include <vfs.h>

/*****************************************************************************
 * __cons_rcall_wrapper
 */

char *__cons_rcall_wrapper(uint64_t source, uint32_t index, int argc, char **argv) {
	struct vfs_obj *dir, *new_fobj;
	const char *name;
	uint64_t rp;
	int type;

	/* check request */
	if (argc <= 2) {
		return NULL;
	}

	/* make sure active driver can construct new objects */
	if (!_vfs_cons) {
		return strdup("! nosys");
	}

	name = argv[1];
	type = atoi(argv[2]);

	/* get requested parent directory */
	dir = vfs_get(index);

	if (dir) {
		/* check permissions */
		if ((acl_get(dir->acl, getuser(RP_PID(source))) & PERM_WRITE) == 0) {
			return strdup("! denied");
		}
		else if ((dir->type & RP_TYPE_DIR) == 0) {
			return strcup("! type");
		}
		else {
			
			/* construct new object */
			new_fobj = _vfs_cons(source, type);

			if (new_fobj) {
				
				/* add new object to parent directory */
				new_fobj->name = strdup(name);
				
				if (vfs_push(source, dir, new_fobj)) {

					/* free the new object */
					if (_vfs_free) {
						_vfs_free(source, new_fobj);
					}
					else {
						acl_free(fobj->acl);
						free(fobj->name);
						free(fobj);
					}

					return NULL;
				}
				else {
					/* return pointer to new object on success */
					rp = RP_CONS(getpid(), new_fobj->index);
				}
			}
			else {
				return strdup("! construct");
			}
		}
	}
	else {
		return strdup("! nfound");
	}

	if (rp) {
		return saprintf("%d %d", RP_PID(rp), RP_INDEX(rp));
	}
	else {
		return NULL;
	}
}
