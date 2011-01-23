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

int main(int argc, char **argv) {
	struct goban *goban;
	int x, y, color, move;

	goban = goban_cons();

	color = COL_B;
	move  = 1;

	print_goban(goban);
	while (1) {

		printf("%s(%d): ", (color == COL_B) ? "black" : "white", move);
		read_move(&x, &y);

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
