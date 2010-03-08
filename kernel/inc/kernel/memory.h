#ifndef MEMORY_H
#define MEMORY_H

#include <flux/arch.h>

/****************************************************************************
 * Page Frame
 *
 * This type represents the physical address of a single frame of memory
 * along with the flags interpreted by the paging system for it.
 */

typedef uintptr_t frame_t;

frame_t frame_alloc();
void    frame_free(frame_t frame);

void    frame_bitmap_set (frame_t frame, bool val);
bool    frame_bitmap_get (frame_t frame);
frame_t frame_bitmap_next(frame_t start);

#define FRAME_PRES  0x001
#define FRAME_READ  0x001
#define FRAME_WRITE 0x002
#define FRAME_USER  0x004
#define FRAME_SUPER 0x000
#define FRAME_SETTYPE(f, t) (((f) & ~0xF) | t)

/****************************************************************************
 * Address Space (page directory)
 *
 * This type represents a paging structure that controls the entire address
 * space.
 */

typedef frame_t space_t;

space_t space_alloc(void);
space_t space_copy (space_t space);
space_t space_load (space_t space);
space_t space_save (void);
space_t space_free (space_t space);

extern space_t  space_curr;
extern uint32_t m_space;

/* refers to currently loaded space */
#define KSPACE 0xFFFFFFFF

/****************************************************************************
 * Page Operations
 */

bool    page_check(space_t space, void *vaddr);
void    page_set  (space_t space, void *vaddr, frame_t val);
void    page_flush(space_t space, void *vaddr);
frame_t page_get  (space_t space, void *vaddr);

/****************************************************************************
 * Address Space Segment
 *
 * In order to keep things organized, Flux breaks up the address space into
 * fixed size chunks called segments. This is unrelated to x86 segmentation.
 * On the x86, the segment size is 4 MB, which corresponds to one page table.
 *
 * Segments are intended to hold only one type of page, such as an area for
 * stack, an area for heap, an area for text, an area for data, or an area
 * for shared library text. Segments can be shared across address spaces using
 * the "load" system call. This can be used to implement shared libraries or
 * shared memory.
 *
 * Segments can be requested from userspace: the allocator for them is 
 * completely in kernelspace.
 */

void    *seg_alloc(space_t space, uint16_t type);
void     seg_touch(space_t space, void *addr);
uint16_t seg_check(space_t space, void *addr);
void     seg_free (space_t space, void *addr);

#define SEGMENT_SIZE  0x00400000
#define SEGMENT_PAGES 0x00000400

/* Segment types */
#define SEGMENT_FLAG_LINK    0x100
#define SEGMENT_FLAG_SUPER   0x200
#define SEGMENT_FLAG_COPY    0x400

#define SEGMENT_TYPE_USER    0x000	/* User program */
#define SEGMENT_TYPE_SHARE   0x100	/* Shared library */
#define SEGMENT_TYPE_KTABLE  0x200	/* Process-owned kernel table */
#define SEGMENT_TYPE_KERNEL  0x300	/* Static kernel stuff */
#define SEGMENT_TYPE(f) ((f) & 0x700)
#define SEGMENT_SETTYPE(f, t) (((f) & ~0x700) | t)

/* Fixed address segments */
#define SEGMENT_ADDR_MIRROR  0xFFC00000	/* Paging mirror */
#define SEGMENT_ADDR_TMIRROR 0xFF800000 /* Temporary paging mirror */
#define SEGMENT_ADDR_TEMP    0xFE000000 /* Temporary pages for stuff */
#define SEGMENT_ADDR_PROC    0xFC400000	/* Process table */
#define SEGMENT_ADDR_THREAD  0xFC000000	/* Thread table */
#define SEGMENT_ADDR_KERNEL  0xF0000000	/* Kernel text/data */

/****************************************************************************
 * High Level Memory Management
 */

void mem_alloc(void *base, uintptr_t size);
void mem_free (void *base, uintptr_t size);

/****************************************************************************
 * Temporary Pages
 *
 * These functions allocate virtual memory in a thread safe fashion from the
 * segment SEGMENT_ADDR_TEMP.
 */

void *temp_alloc(void);
void  temp_free(void *addr);

extern uint32_t m_temp[PAGESZ / sizeof(frame_t)];

#endif
