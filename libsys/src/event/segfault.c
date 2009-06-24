#include <lib.h>
#include <event.h>

void segfault() {
	eout("Segmentation fault\n");
	exit_call(0);
}
