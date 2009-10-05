/* Copyright 2009 Nick Johnson */

#ifndef MEM_H
#define MEM_H

/* General virtual memory map:
0x00000000 - 0xF7FFFFFF: userspace 		(user, read-write, cloned)
	0x00001000: process image
	0xF0000000: libc image
	0xF7FF0000: standard stack
	0xF7FFC000: state saving stack
	0xF7FFF000: signal handler table
0xF8000000 - 0xFEFFFFFF: libspace 		(user, readonly, linked)
	0xF8000000: signal override table
	0xF8001000: static libsys data
	0xFC000000: libsys image
0xFF000000 - 0xFFFFFFFF: kernelspace 	(kernel, linked)
	0xFF000000: remapped lower memory
	0xFF100000: kernel image 
	0xFF400000: task table
	0xFF500000: none/various
	0xFF800000: temporary map
	0xFFC00000: resident map
*/

#include <khaos/config.h> /* Contains macros for VM layout */

/***** PAGE FLAGS *****/

/* Normal page flags */
#define PF_PRES 0x1		/* Is present */
#define PF_RW   0x2		/* Is writeable */
#define PF_USER 0x4		/* Is user-mode */
#define PF_WRTT 0x8		/* Write-through enabled */
#define PF_DISC 0x10	/* Cache disabled */
#define PF_DIRT 0x20	/* Is dirty */
#define PF_ACCS 0x40	/* Has been accessed */

/* Special page flags */
#define PF_LINK 0x200	/* Is linked from elsewhere (do not free frame) */
#define PF_REAL 0x400	/* Is linked to elsewhere (do not free at all) */
#define PF_SWAP 0x800	/* Is swapped out */

#define PF_MASK 0x0E7F	/* Page flags that can be used */

/***** DATA TYPES *****/
typedef uint32_t page_t;
typedef uint32_t ptbl_t;
typedef uint32_t map_t;

/****** MAP.C *****/
void  map_temp(map_t map);				/* Puts a map in temporary space */
map_t map_alloc(void);					/* Allocates a new map */
map_t map_free(map_t map);				/* Frees a map (does not clean) */
map_t map_clean(map_t map);				/* Cleans a map (frees all *user* memory) */
map_t map_clone(void);					/* Clones the current map */
extern map_t map_load(map_t map);		/* Activates a new map */
void map_gc(uint32_t page);				/* Frees unused page tables */

/***** FRAME.C ******/
extern pool_t *fpool;

#define frame_new() (pool_alloc(fpool) << 12)			/* Allocates a new frame	 */
#define frame_free(addr) (pool_free(fpool, addr >> 12))	/* Frees a frame */

/***** PAGE.C *****/
extern ptbl_t *cmap, *tmap;					/* Address of current page directory */
extern page_t *ctbl, *ttbl;					/* Base of current page tables */
extern uint32_t *tsrc, *tdst;

void   mem_alloc(uintptr_t base, uintptr_t size, uint16_t flags);
void   mem_free(uintptr_t base, uintptr_t size);

void   page_touch(uint32_t page);				/* Makes sure a page exists */
void   page_set(uint32_t page, page_t value);	/* Sets the value of a page */
page_t page_get(uint32_t page);					/* Returns the value of a page */
void   page_flush(uint32_t addr);				/* Flushes the TLB for a page */
void   temp_touch(uint32_t page);				/* Makes sure a temporary page exists */
void   temp_set(uint32_t page, page_t value);	/* Sets the value of a temporary page */
page_t temp_get(uint32_t page);					/* Returns the value of a temporary page */
#define page_fmt(base,flags) (((base)&0xFFFFF000)|((flags)&PF_MASK))
#define page_ufmt(page) ((page)&0xFFFFF000)
#define p_alloc(addr, flags) (page_set(addr, page_fmt(frame_new(), (flags & PF_MASK) | PF_PRES)))
#define p_free(addr) do { frame_free(page_ufmt(page_get(addr))); page_set(addr, 0); } while(0);

#endif /*MEM_H*/
