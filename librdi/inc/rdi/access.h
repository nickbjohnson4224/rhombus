/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#ifndef _RDI_ACCESS_H
#define _RDI_ACCESS_H

#include <rdi/core.h>
#include <natio.h>

/* soon to be removed */
int vfs_permit(struct resource *r, uint64_t source, int operation);

/*****************************************************************************
 * RDI access control
 *
 * The RDI security model uses a series of access control lists. Each ACL
 * corresponds to a property of the caller, in this case PID and UID. There is
 * also a default access bitmap that is always in effect. The effective access
 * bitmap of a caller on a file is the access bitmaps of all ACLs ANDed 
 * together.
 *
 * For example, if a driver wanted to grant write access to a file to a
 * specific set of users, the default access bitmap would be 
 * PERM_READ | PERM_WRITE, the PID ACL would have default value
 * PERM_READ | PERM_WRITE and no contents, and the UID ACL would have default
 * value PERM_READ with select entries having PERM_READ | PERM_WRITE.
 *
 * The PID ACL tends not to be saved to any persistent medium, since PIDs are
 * not persistent. In addition, the PID ACL cannot be directly controlled by
 * user requests (rcall setaccess); usually, it is only modified when a file
 * is opened or closed, to implement locks or some sort of lockout mechanism.
 *
 * The default access bitmap is generally only set by resource_cons, and is
 * supposed to represent the extent of the operations that can ever be done on
 * a resource (a read-only filesystem, for example, would have only PERM_READ)
 * When a resource is initialized, all other ACLs have default bitmaps with
 * all access, such that the default access bitmap is the access for 
 *
 * Functions:
 *
 *   rdi_access determines whether a particular operation or set of operations
 *   is legal for <source> to perform on a resource <r>.
 *
 *   rdi_set_foo_access sets the access bitmap in ACL foo, and
 *   rdi_set_foo_access_dfl sets the default access bitmap in ACL foo.
 *
 *   rdi_set_dfl_access sets the default access bitmap for the resource.
 *
 * Access bitmaps generally only contain three relevant bits: PERM_READ, 
 * PERM_WRITE, and PERM_ALTER (defined in <natio.h>.) However, it is 
 * guaranteed that RDI can accept up to eight bits total in an access bitmap.
 * Drivers may use these free bits for any purpose, but they will be otherwise
 * ignored by RDI.
 */

int  rdi_access(struct resource *r, uint64_t source, int operation);

void rdi_set_uid_access    (struct resource *r, uint32_t uid, int access);
void rdi_set_uid_access_dfl(struct resource *r, int access);
void rdi_set_pid_access    (struct resource *r, uint32_t pid, int access);
void rdi_set_pid_access_dfl(struct resource *r, int access);
void rdi_set_dfl_access    (struct resource *r, int access);

#endif/*_RDI_ACCESS_H*/
