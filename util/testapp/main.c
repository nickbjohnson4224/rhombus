#include <wmanager.h>
#include <stdlib.h>
#include <ipc.h>
#include <page.h>
#include <string.h>
#include <stdio.h>
#include <natio.h>

const int width = 256;
const int height = 30;

int main(int argc, char **argv) {
	size_t size = width * height * 4;
	uint8_t *bitmap = malloc(size);
	memset(bitmap, 0, size);

	if (wm_init()) {
		fprintf(stderr, "%s: error: cannot initialize window manager\n", argv[0]);
		return 1;
	}

	wm_set_bitmap(0, bitmap, size);
	wm_add_window(0, width, height, 0);
	for (int i = 0; i <= 0xff; i++)	{
		for (int line = 0; line < 3; line++) {
			for (int timer = 0; timer < 0xfff; timer++) {
				for (int j = 0; j < 10; j++) {
					for (int c = 0; c < 3; c++) {
						bitmap[(i + (10 * line + j) * width) * 4 + c] = line == c ? i : 0;
					}
					bitmap[(i + (10 * line + j) * width) * 4 + 3] = 0xff;
				}
			}
		}
	}

	return 0;
}
