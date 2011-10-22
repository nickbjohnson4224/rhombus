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

#ifndef __LIBRDI_CORE_H
#define __LIBRDI_CORE_H

#include <rho/natio.h>

#include <rdi/robject.h>

/*****************************************************************************
 * Rhombus Driver Interface (RDI)
 *
 * RDI is a framework built using robjects that provides support to drivers
 * for a variety of tasks, including permissions, VFS management, and I/O.
 * It is composed of a set of core robject classes, which are modified by the
 * driver to incorporate driver code.
 */

void rdi_init();

/*****************************************************************************
 * rdi_class_core (extends robject_class_basic) - driver
 *
 * All RDI objects inherit from this same base class, which contains
 * implementations of the following calls from the driver:
 *
 * Calls:
 *
 * cons <type> (additional args based on type) - none
 *   
 *   Requests the creation of a new robject of the given type. The three valid
 *   types are "file", "dir", and "link".
 *
 *   Default behavior is to construct the new robject unconditionally. This is
 *   not at all secure.
 *
 *   Return: a robject pointer to the new robject
 *
 * sync - W
 *
 *   Requests that the robject be in some way synchronized. This is open for
 *   per-driver interpretation, but tends to imply a sync to disk if 
 *   applicable.
 *
 *   Default behavior is a no-op.
 */

extern struct robject *rdi_class_core;
void __rdi_class_core_setup();

struct robject *rdi_core_cons(uint32_t index, uint32_t access);
void            rdi_core_free(struct robject *r);

typedef struct robject *(*rdi_cons_hook)(rp_t src, int argc, char **argv);

extern rdi_cons_hook rdi_global_cons_file_hook;
extern rdi_cons_hook rdi_global_cons_dir_hook;
extern rdi_cons_hook rdi_global_cons_link_hook;

#endif/*__LIBRDI_CORE_H*/
