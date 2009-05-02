// Copyright 2009 Nick Johnson

#include <lib.h>
#include <task.h>

struct sysmap *sysmap;

void init_sysmap() {
	sysmap = kmalloc(sizeof(struct sysmap));
	memclr(sysmap, sizeof(struct sysmap));
}
