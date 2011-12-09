#include <stdlib.h>

int foo = 42;

int bar(int x) __attribute__ ((noinline));
int bar(int x) {
	int y = foo;
	foo = x;
	return y;
}

int bar1(int x) {
	return bar(x);
}

void *baz(void) {
	return malloc(foo);
}
