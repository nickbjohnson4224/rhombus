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

#include <time.h>
#include <stdio.h>
#include <natio.h>
#include <stdlib.h>
#include "calico.h"

static struct goban *playout(struct goban *goban, int moves) {
	struct goban *temp;
	int i;
	int x, y, color;

	temp = goban_clone(goban);

	color = COL_B;
	i = 0;
	while (i < moves) {
		x = rand() % 19;
		y = rand() % 19;
		
		if (goban_play(temp, x, y, color)) {
			continue;
		}
		else {
			color = (color == COL_W) ? COL_B : COL_W;
			i++;
		}
	}

	return temp;
}
/****************************************************************************
 * ilog2
 *
 * Returns the ceiling of the base 2 logarithm of the given integer. Uses a 
 * unrolled version of the algorithm detailed at 
 * <http://graphics.stanford.edu/~seander/bithacks.html#IntegerLog>.
 */

static uint8_t ilog2(uintptr_t n) {
	register uint8_t r = 0;
	uintptr_t orig_n = n;

	if (n &   0xFFFF0000) {
		n >>= 0x10;
		r |=  0x10;
	}
	if (n &   0xFF00) {
		n >>= 0x08;
		r |=  0x08;
	}
	if (n &   0xF0) {
		n >>= 0x04;
		r |=  0x04;
	}
	if (n &   0xC) {
		n >>= 0x02;
		r |=  0x02;
	}
	if (n &   0x2) {
		n >>= 0x01;
		r |=  0x01;
	}

	if (((uintptr_t) 1 << r) == orig_n) {
		return r;
	}
	else {
		return r + 1;
	}
}
int main(int argc, char **argv) {
	struct goban *goban;
	int x, y, color, move;

	goban = goban_cons();
	color = COL_B;
	move  = 1;

	print_goban(goban);
	while (1) {

		printf("%s(%d): ", (color == COL_B) ? "black" : "white", move);
		if (read_move(&x, &y)) return 0;

		if (goban_check(goban, x, y, color)) {
			printf("illegal move\n");
		}
		else {
			goban_play(goban, x, y, color);
			color = (color == COL_W) ? COL_B : COL_W;
			move++;

			print_goban(goban);
		}
	}

	return 0;
}
