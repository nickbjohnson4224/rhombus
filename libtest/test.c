#include <stdlib.h>

int foo = 42;

int bar(int x) __attribute__ ((noinline));
int bar(int x) {
	int y = foo;
	foo = x;
	return y;
}

int bar1(int x) {
	return 1 + bar(x - 1);
}

int __zab(int);
int baz(int x) {
	return __zab(x + 1);
}
