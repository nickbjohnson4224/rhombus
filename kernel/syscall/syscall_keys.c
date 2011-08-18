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

#include <interrupt.h>
#include <process.h>

/*****************************************************************************
 * syscall_keys (int 0x54)
 *
 * EBX: operation
 * ECX: value
 * EDX: target
 * EDI: resource PID
 * ESI: resource index
 *
 * KEY_CHECK - 0:
 *   Check the key value of a given key from <target>. If <target> is zero,
 *   the current process is used instead.
 *
 * KEY_GRANT - 1:
 *   Create key with given value and grant to target. This key is a new key, 
 *   and must have the same resource PID as the caller.
 *
 * KEY_SHARE - 2:
 *
 * KEY_DELETE - 3:
 *
 * KEY_RECALL - 4:
 *   
 */

struct thread *syscall_keys(struct thread *image) {
	
	return image;
}
