// Copyright 2009 Nick Johnson

/* General virtual memory map:
0x00000000 - 0xF3FFFFFF: userspace
0xF4000000 - 0xF7FFFFFF: libspace
0xF8000000 - 0xFFFFFFFF: kernelspace
	0xF8000000: lower memory
	0xF807C000: BIOS
	0xF8100000: kernel image
	0xF8400000: kernel heap
	0xFFFF0000: temporary
*/

#ifndef MEM_H
#define MEM_H

// Page
typedef u32int page_t;

// Page table
typedef page_t* ptbl_t;

// Page directory / "map"
typedef struct map {
	u32int *pdir;	// Physical addresses of tables
	ptbl_t *virt;	// Virtual addresses of tables
} map_t;

/****** SYSTEM CALLS *****/
u32int mmap(u32int base, u32int flags);	// Map memory to a page
u32int umap(u32int base);				// Unmap memory from a page
u32int rmap(u32int base, u32int src);	// Move a mapped frame

/***** PRIVILEDGED SYSTEM CALLS *****/
u32int grab(u32int base, u32int src);	// Get a frame by force
u32int phys(u32int base, u32int task);	// Get physical address

/***** ALLOC.C *****/
pool_t *ppool;
u32int proto_base;
// These are PAGE ALIGNED ALLOCATORS ONLY!
void *kmalloc(u32int size);	// Allocates one page
void kfree(void *addr);		// Frees one page

/***** MAP.C *****/
#define MF_COPY_LIBOS 	0x01	// Copy libspace as well (recommended)
#define MF_CLEAR_USER 	0x02	// Do not copy userspace

map_t kmap;
map_t *map_alloc(map_t *map);							// Allocates a new map
map_t *map_free(map_t *map);							// Frees a map (does not clean)
map_t *map_clean(map_t *map);							// Cleans a map (frees all *user* memory)
map_t *map_clone(map_t *dest, map_t *src, u8int flags);	// Clones a map, and links
map_t *map_load(map_t *map);							// Activates a new map
map_t *map_enum(map_t *map);							// Prints contents of a map (pdirs only)

/***** PAGE.C *****/
char mem_setup;
page_t page_touch(map_t *map, u32int page);				// Makes sure a page exists
page_t page_set(map_t *map, u32int page, page_t value);	// Sets the value of a page
page_t page_get(map_t *map, u32int page);				// Returns the value of a page
u32int phys_of(map_t *map, void *addr);					// Gets the physical address of a pointer
u32int p_alloc(map_t *map, u32int addr, u32int flags);	// Allocate a page to a position
u32int p_free (map_t *map, u32int addr);				// Free a page from a position 
#define PF_MASK 0x0C67									// Page flags that can be used
#define page_fmt(base,flags) ((base&0xFFFFF000)|(flags&PF_MASK))
#define page_ufmt(page) (page&0xFFFFF000)

/***** FRAME.C ******/
pool_t *fpool;
u32int frame_new();				// Allocates a new frame
void frame_free(u32int addr);	// Frees a frame

/***** FLAGS *****/

// Normal page flags
#define PF_PRES 0x1		// Is present
#define PF_RW   0x2		// Is writeable
#define PF_USER 0x4		// Is user-mode
#define PF_DIRT 0x20	// Is dirty
#define PF_ACCS 0x40	// Has been accessed

// Special page flags
#define PF_LINK 0x200	// Is linked from elsewhere (do not free frame)
#define PF_REAL 0x400	// Is linked to elsewhere (do not free at all)
#define PF_SWAP 0x800	// Is swapped out

#endif /*MEM_H*/
