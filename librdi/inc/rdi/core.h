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

#ifndef _RDI_CORE_H
#define _RDI_CORE_H

#include <robject.h>

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
 * find <path> - R
 *
 *   Attempt to locate the robject with the given path relative to this one.
 *
 *   Default behavior is to return a pointer to the called robject. This is
 *   overridden by most subclasses, however.
 *
 *   Return: "%r" <pointer to robject>
 *
 * cons <type> (additional args based on type) - none
 *   
 *   Requests the creation of a new robject of the given type. 
 *
 *   Default behavior is to construct the new robject unconditionally. This is
 *   not at all secure.
 *
 *   Return: a robject pointer to the new robject
 * 
 * get-access <user> - R
 *
 *   Requests the access bitmap of the robject for a given user ID. This 
 *   "bitmap" is actually a hexadecimal string value. See natio.h for details 
 *   on its contents.
 *
 *   Default behavior is to return the contents of data field 
 *   "access-%X" <user>, if it exists, and otherwise to return 0.
 *
 *   Return: an access bitmap (hexadecimal string) valid for the given user.
 *
 * set-access <user> <bitmap> - A
 *
 *   Requests the the access bitmap of the robject for a given user ID be
 *   changed.
 *
 *   Default behavior is to allow the request only if the source user has
 *   PERM_ALTER granted to them for this robject.
 *
 * sync - W
 *
 *   Requests that the robject be in some way synchronized. This is open for
 *   per-driver interpretation, but tends to imply a sync to disk if 
 *   applicable.
 *
 *   Default behavior is a no-op.
 *
 * Fields:
 *
 * access-%X <uid>
 *
 *   See calls get-access and set-access for details.
 *
 *   Type: uint32_t
 */

extern struct robject *rdi_class_core;
void __rdi_class_core_setup();

struct robject *rdi_core_cons(uint32_t index, uint32_t access);
void            rdi_core_free(struct robject *r);

/******************************************************************************
 * rdi_class_event (extends rdi_class_core) - event
 *
 * Calls:
 *
 * subscribe (target) - R
 *
 * unsubscribe (target) - W
 */

extern struct robject *rdi_class_event;
void __rdi_class_event_setup();

struct robject *rdi_event_cons(uint32_t index, uint32_t access);
void            rdi_event_free(struct robject *r);

#endif/*_RDI_CORE_H*/
