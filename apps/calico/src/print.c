/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdio.h>
#include "calico.h"

int print_goban(struct goban *goban) {
	int x, y;
	const char *letters = "ABCDEFGHJKLMNOPQRST";

	printf("\n    ");
	for (x = 0; x < 19; x++) {
		printf("%c ", letters[x]);
	}
	printf("\n");

	for (y = 0; y < 19; y++) {

		printf("%3d ", 19 - y);

		for (x = 0; x < 19; x++) {

			if (goban_get_color(goban, x, y) == COL_W) {
				printf("O ");
			}
			else if (goban_get_color(goban, x, y) == COL_B) {
				printf("X ");
			}
			else {
				if ((x - 3) % 6 == 0 && (y - 3) % 6 == 0) {
					printf("+ ");
				}
				else {
					printf("- ");
				}
			}
		}

		if (y == 0) {
			printf("%-2d\tblack: %d\twhite: %d\n", 19 - y, goban->b_caps, goban->w_caps);
		}
		else {
			printf("%-2d\n", 19 - y);
		}
	}

	printf("    ");
	for (x = 0; x < 19; x++) {
		printf("%c ", letters[x]);
	}
	printf("\n\n");
	return 0;
}
