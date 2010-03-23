/* 
 * Copyright 2009 Nick Johnson 
 * ISC Licensed, see LICENSE for details 
 */

#include <lib.h>
#include <mem.h>

frame_t *cmap = (void*) (PGE_MAP + 0x3FF000);	/* Current page directory mapping */
frame_t *ctbl = (void*) PGE_MAP;				/* Current base page table mapping */
uint32_t *tsrc = (void*) TMP_SRC;				/* Some pages of open virtual memory */
uint32_t *tdst = (void*) TMP_DST;				/* Some pages of open virtual memory */
