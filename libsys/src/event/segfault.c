#include <lib.h>
#include <event.h>

void segfault() {
	eout("\nSegmentation fault\n");
	for(;;);
	exit_call(0);
}
