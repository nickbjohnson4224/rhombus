/* Copyright 2010 Nick Johnson */

#ifndef SPACE_H
#define SPACE_H

#include <flux/config.h>
#include <flux/arch.h>

/***** PAGE FLAGS *****/

/* MMAP flags */
#define MMAP_CMASK	0x0BB /* Capability mask */

#define MMAP_READ	0x001
#define MMAP_WRITE	0x002
#define MMAP_EXEC	0x004
#define MMAP_FREE	0x008
#define MMAP_FRAME	0x010
#define MMAP_PHYS	0x020
#define MMAP_MOVE	0x040
#define MMAP_LEAVE  0x080

/* Normal page flags */
#define PF_PRES 0x1		/* Is present */
#define PF_RW   0x2		/* Is writeable */
#define PF_USER 0x4		/* Is user-mode */
#define PF_WRTT 0x8		/* Write-through enabled */
#define PF_DISC 0x10	/* Cache disabled */
#define PF_DIRT 0x20	/* Is dirty */
#define PF_ACCS 0x40	/* Has been accessed */

#define PF_MASK 0x0E7F	/* Page flags that can be used */

/***** FRAME ALLOCATOR *****/

typedef uint32_t frame_t;

void    frame_init(uintptr_t memsize);
frame_t frame_new (void);
void    frame_free(frame_t frame);

/***** ADDRESS SPACES *****/

typedef uint32_t space_t;

void    space_exmap(uintptr_t seg, space_t space);
space_t space_alloc(void);
space_t space_clone(void);
void    space_free (space_t space);
void    space_load (space_t space);

extern frame_t *cmap;		/* Address of current page directory */
extern frame_t *ctbl;		/* Base of current page tables */

/***** HIGH LEVEL MEMORY OPERATIONS *****/

void   mem_init(void);
void   mem_alloc(uintptr_t base, uintptr_t size, uint16_t flags);
void   mem_free (uintptr_t base, uintptr_t size);

/***** ADDRESS SPACE SEGMENTS *****/

#define SEGSZ 0x400000

#define SEG_HIGH 0x001 /* Segment is as high as possible */

#define SEG_LINK 0x200 /* Segment is linked when cloning */
#define SEG_USED 0x400 /* Segment is allocated and in use */
#define SEG_ALLC 0x800 /* Segment allocated by segment_alloc or page_touch */

uintptr_t segment_alloc(uint32_t type);
void      segment_free (uintptr_t seg);

/***** PAGE OPERATIONS *****/

void    page_touch(uintptr_t page);
void    page_set  (uintptr_t page, frame_t value);
frame_t page_get  (uintptr_t page);
void    page_flush(uintptr_t addr);
void    page_flush_full(void);

void	page_extouch(uintptr_t seg, uintptr_t page);
void	page_exset  (uintptr_t seg, uintptr_t page, frame_t value);
frame_t page_exget  (uintptr_t seg, uintptr_t page);

#define page_fmt(base,flags) (((base)&0xFFFFF000)|((flags)&PF_MASK))
#define page_ufmt(page) ((page)&0xFFFFF000)

/***** KERNEL HEAP *****/

void *heap_alloc(size_t size);
void  heap_free(void *ptr, size_t size);
void  heap_new_slab(size_t bucket);
void *heap_valloc(void);

#endif /*SPACE_H*/
