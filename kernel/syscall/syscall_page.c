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

#include <interrupt.h>
#include <string.h>
#include <thread.h>
#include <space.h>
#include <debug.h>

/*****************************************************************************
 * syscall_page (int 0x46)
 *
 * EBX: address
 * ECX: count
 * EDX: perm
 * ESI: source
 * EDI: offset
 *
 * Maps memory from the source <source> to the memory region from <address> to
 * <address> + <count> * PAGESZ, ensuring that the memory has at leas the
 * permissions <perm>. Depending on the source, <offset> may be relevant. 
 * Returns zero on success, nonzero on error.
 *
 * Possibilities for <source>:
 *
 * PAGE_NULL - 0
 *     Map null frames to the region, freeing existing frames.
 *
 * PAGE_ANON - 1
 *     Map anonymous memory to the region. This memory has undefined contents,
 *     but is guaranteed not to be in use by any other processes.
 *
 * PAGE_PACK - 2
 *     Map the contents of the current thread's packet (if it exists) to the
 *     region. If the packet is larger than the region, only part of it is
 *     mapped. If the packet is smaller than the region, part of the region is
 *     left empty. Either way, the packet is then freed and becomes 
 *     inaccessible.
 *
 * PAGE_PHYS - 3
 *     Map the physical memory region from <offset> to <offset> + <count> *
 *     PAGESZ to the virtual memory region. This is a privileged function.
 *
 * PAGE_SELF - 4
 *     Map the virtual memory region from <offset> to <offset> + <count> *
 *     PAGESZ to the requested virtual memory region. This function can be
 *     used to make shared memory regions: make a copy of a region, then send
 *     it as the packet of a message. This function ignores <perm>, instead
 *     using the permission bits of the source region.
 *
 * PAGE_PROT - 5
 *     Change the permissions on the memory region from <offset> to <offset> +
 *     <count> * PAGESZ to the requested permissions.
 *
 * Flags for <perm>:
 *
 * PROT_READ  - 1
 *     Allow page to be read.
 *
 * PROT_WRITE - 2
 *     Allow page to be written.
 *
 * PROT_EXEC  - 4
 *     Allow execution from the page.
 *
 * PROT_LOCK  - 8
 *     Prevent permissions from being changed further. This is useful for
 *     shared memory, to prevent the reciever from changing the permissions of
 *     a granted memory region.
 *
 * PROT_LINK  - 16
 *     Page is linked instead of copied during fork(), creating a shared 
 *     memory region. This is used for shared libraries (and shared memory
 *     in general) to prevent duplication of those shared resources.
 */

struct thread *syscall_page(struct thread *image) {
	uintptr_t address;
	uintptr_t count;
	uintptr_t perm;
	uintptr_t source;
	uintptr_t offset;
	uintptr_t i;

	address = image->ebx;
	count   = image->ecx;
	perm    = image->edx;
	source  = image->esi;
	offset  = image->edi;

	/* check for alignment errors */
	if (address & 0xFFF) {
		image->eax = 1;
		return image;
	}

	/* check bounds of region */
	if (address >= KSPACE || address + (count * PAGESZ) >= KSPACE) {
		image->eax = 1;
		return image;
	}

	/* reject insane requests (> 64MB) */
	if (count > 0x4000) {
		image->eax = 1;
		return image;
	}

	/* change <perm> to real page flags */
	perm = PF_USER | PF_PRES 
		| ((perm & 2)  ? PF_RW   : 0) 
		| ((perm & 8)  ? PF_LOCK : 0) 
		| ((perm & 16) ? PF_LINK : 0);

	switch (source) {
	case 0: /* PAGE_NULL */
		
		/* free all allocated pages in the region, set the frames to zero */
		for (i = address; i < address + count * PAGESZ; i += PAGESZ) {
			if (page_get(i) & PF_PRES) {
				frame_free(page_ufmt(page_get(i)));
				page_set(i, 0);
			}
		}

		break;
	case 1: /* PAGE_ANON */

		/* check for out of memory error */
		if (out_of_memory) {
			image->eax = 1;
			return image;
		}

		/* allocate requested pages, but don't free old ones if they're there */
		for (i = address; i < address + count * PAGESZ; i += PAGESZ) {
			if ((page_get(i) & PF_PRES) == 0) {
				page_set(i, page_fmt(frame_new(), perm));
			}
		}

		break;
	case 2: /* PAGE_PACK */

		/* reject if there is no packet */
		if (!image->msg) {
			image->eax = 1;
			return image;
		}

		/* calculate actual page count */
		if (count > image->msg->count) {
			count = image->msg->count;
		}
		
		/* map the packet */
		for (i = 0; i < count; i++) {

			/* free encumbering frames */
			if (page_get(address + i * PAGESZ) & PF_PRES) {
				frame_free(page_ufmt(page_get(address + i * PAGESZ)));
			}
			
			/* map frame from packet */
			page_set(address + i * PAGESZ, page_fmt(image->msg->frame[i], perm | PF_LOCK));
		}

		/* free any unused packet contents */
		for (; i < image->msg->count; i++) {
			frame_free(image->msg->frame[i]);
		}

		/* free the message packet structure */
		heap_free(image->msg->frame, image->msg->count * sizeof(uint32_t));
		heap_free(image->msg, sizeof(struct msg));
		image->msg = NULL;

		break;
	case 3: /* PAGE_PHYS */
		
		/* check effective permissions */
		if (image->user) {
			image->eax = 1;
			return image;
		}

		/* allocate requested frames */
		for (i = 0; i < count * PAGESZ; i += PAGESZ) {

			/* free encumbering frames */
			if (page_get(address + i) & PF_PRES) {
				frame_free(page_ufmt(page_get(address + i)));
			}

			page_set(address + i, page_fmt(offset + i, perm));
		}

		break;
	case 4: /* PAGE_SELF */

		/* check for alignment errors */
		if (offset & 0xFFF) {
			image->eax = 1;
			return image;
		}

		/* check bounds of region */
		if (offset >= KSPACE || offset + (count * PAGESZ) >= KSPACE) {
			image->eax = 1;
			return image;
		}

		/* copy frames */
		for (i = 0; i < count; i++) {
			
			/* skip if source not present */
			if ((page_get(offset + i * PAGESZ) & PF_PRES) == 0) {
				continue;
			}

			/* free encumbering frames */
			if (page_get(address + i * PAGESZ)) {
				frame_free(page_ufmt(page_get(address + i * PAGESZ)));
			}

			/* copy frame and increment reference count */
			page_set(address + i * PAGESZ, page_get(offset + i * PAGESZ));
			frame_ref(page_get(offset + i * PAGESZ));
		}

		break;
	case 5: /* PAGE_PROT */
		
		/* set permissions on frames */
		for (i = address; i < address + count * PAGESZ; i += PAGESZ) {
			
			/* skip empty frames */
			if ((page_get(i) & PF_PRES) == 0) {
				continue;
			}

			/* skip locked frames */
			if (page_get(i) & PF_LOCK) {
				continue;
			}

			/* set new permissions */
			page_set(i, page_fmt(page_get(i), perm));
		}

		break;
	default:
		image->eax = 1;
		return image;
	}

	image->eax = 0;
	return image;
}
