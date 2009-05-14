int func() {
	for(;;);

int main() {
	int *a = (int*) 0x42242442;
	*a = 0;
	a++;
	for(;;) {
		func();
	}
}
