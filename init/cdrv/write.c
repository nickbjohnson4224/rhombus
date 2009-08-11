#include <lib.h>

char *wrtbuffer = (void*) 0x30000000;
void write_handler(u32int target, addr_t base, u32int size) {
	fmap(target, (addr_t) wrtbuffer, base, size, 0x7);
	char *wbase = (void*) ((base % PAGESZ) + (u32int) wrtbuffer);

	u32int i;
	for (i = 0; i < size; i++)
		printf("%c", wbase[i]);
	curse(-1, -1);
}
