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

#ifndef SPACE_H
#define SPACE_H

#include <stdbool.h>
#include <stdint.h>
#include <types.h>
#include <arch.h>

/* page flags ***************************************************************/

#define PF_PRES 0x1		/* Is present */
#define PF_RW   0x2		/* Is writeable */
#define PF_USER 0x4		/* Is user-mode */
#define PF_WRTT 0x8		/* Write-through enabled */
#define PF_DISC 0x10	/* Cache disabled */
#define PF_DIRT 0x20	/* Is dirty */
#define PF_ACCS 0x40	/* Has been accessed */
#define PF_LOCK 0x200	/* Permissions locked */
#define PF_COW  0x400   /* Copy on write */
#define PF_LINK 0x800   /* Link, don't copy (on fork) */

#define PF_MASK 0x0E7F	/* Page flags that can be used */

/* frame allocator **********************************************************/

extern bool out_of_memory;

void    frame_add (frame_t frame);
frame_t frame_new (void);
void    frame_ref (frame_t frame);
void    frame_free(frame_t frame);

/* address spaces ***********************************************************/

void    space_exmap(space_t space);
space_t space_alloc(void);
space_t space_clone(void);
void    space_free (space_t space);

extern frame_t *cmap;		/* Address of current page directory */
extern frame_t *ctbl;		/* Base of current page tables */

/* high level memory operations *********************************************/

void   mem_alloc(uintptr_t base, uintptr_t size, uint16_t flags);
void   mem_free (uintptr_t base, uintptr_t size);

/* address space segments ***************************************************/

#define SEGSZ 0x400000

/* page operations **********************************************************/

void    page_touch(uintptr_t page);
void    page_set  (uintptr_t page, frame_t value);
frame_t page_get  (uintptr_t page);

void	page_extouch(uintptr_t page);
void	page_exset  (uintptr_t page, frame_t value);
frame_t page_exget  (uintptr_t page);

#define page_fmt(base,flags) (((base)&0xFFFFF000)|((flags)&PF_MASK))
#define page_ufmt(page) ((page)&0xFFFFF000)

/* kernel heap **************************************************************/

void *heap_alloc(size_t size);
void  heap_free(void *ptr, size_t size);

#endif/*SPACE_H*/
