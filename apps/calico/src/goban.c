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
#include <stdlib.h>
#include <string.h>
#include "calico.h"

static void _group_set_id(struct goban *goban, int grpid_old, int grpid_new);
static int  _group_new_id(struct goban *goban);
static void _group_del_id(struct goban *goban, int grpid);
static void _group_clear (struct goban *goban, int grpid);

struct goban *goban_cons(void) {
	struct goban *goban;
	int i, j;

	goban = malloc(sizeof(struct goban));

	for (i = 0; i < 19; i++) {
		goban->color[i] = 0xFFFFFFC000000000;
		for (j = 0; j < 19; j++) {
			goban->grpid[i * 19 + j] = 0;
		}
	}

	goban->grpbm[0] = 0x01;
	for (i = 1; i < 8; i++) {
		goban->grpbm[i] = 0x00;
	}

	goban->ko_group = 0;
	goban->b_caps   = 0;
	goban->w_caps   = 0;
	return goban;
}

struct goban *goban_clone(struct goban *src) {
	struct goban *goban;

	goban = malloc(sizeof(struct goban));
	memcpy(goban, src, sizeof(struct goban));

	return goban;
}

int goban_group_size(struct goban *goban, int x, int y) {
	int group;

	group = goban->grpid[y * 19 + x];

	return (!group) ? 0 : goban->group[group].size;
}

int goban_group_libs(struct goban *goban, int x, int y) {
	int group;

	group = goban->grpid[y * 19 + x];

	return (!group) ? 0 : goban->group[group].libs;
}

int goban_get_color(struct goban *goban, int x, int y) {

	if (x >= 19 || x < 0 || y >= 19 || y < 0) {
		return COL_O;
	}

	return ((goban->color[y] >> (x * 2)) & 0x3);
}

int goban_get_grpid(struct goban *goban, int x, int y) {

	if (x >= 19 || x < 0 || y >= 19 || y < 0) {
		return 0;
	}

	return goban->grpid[y * 19 + x];
}

int goban_check(struct goban *goban, int x, int y, int color) {
	int opponent;

	opponent = (color == COL_W) ? COL_B : COL_W;

	/* check if space is occupied (if so, fail) */
	if (goban_get_color(goban, x, y) != COL_E) {
		return 1;
	}

	/* check for liberties (if so, succeed) */
	if ((goban_get_color(goban, x, y + 1) == COL_E) ||
		(goban_get_color(goban, x + 1, y) == COL_E) ||
		(goban_get_color(goban, x, y - 1) == COL_E) ||
		(goban_get_color(goban, x - 1, y) == COL_E)) {
			return 0;
	}

	/* check for captures (if so: if ko, fail, otherwise succeed) */
	if (goban_get_color(goban, x, y + 1) == opponent) {
		if (goban_group_libs(goban, x, y + 1) == 1) {
			if (goban_get_grpid(goban, x, y + 1) == goban->ko_group) {
				return 1;
			}
			else {
				return 0;
			}
		}
	}
	if (goban_get_color(goban, x + 1, y) == opponent) {
		if (goban_group_libs(goban, x + 1, y) == 1) {
			if (goban_get_grpid(goban, x + 1, y) == goban->ko_group) {
				return 1;
			}
			else {
				return 0;
			}
		}
	}
	if (goban_get_color(goban, x, y - 1) == opponent) {
		if (goban_group_libs(goban, x, y - 1) == 1) {
			if (goban_get_grpid(goban, x, y - 1) == goban->ko_group) {
				return 1;
			}
			else {
				return 0;
			}
		}
	}
	if (goban_get_color(goban, x - 1, y) == opponent) {
		if (goban_group_libs(goban, x - 1, y) == 1) {
			if (goban_get_grpid(goban, x - 1, y) == goban->ko_group) {
				return 1;
			}
			else {
				return 0;
			}
		}
	}

	/* check for not suicide (if so, succeed) */
	if (goban_get_color(goban, x, y + 1) == color) {
		if (goban_group_libs(goban, x, y + 1) != 1) {
			return 0;
		}
	}
	if (goban_get_color(goban, x + 1, y) == color) {
		if (goban_group_libs(goban, x + 1, y) != 1) {
			return 0;
		}
	}
	if (goban_get_color(goban, x, y - 1) == color) {
		if (goban_group_libs(goban, x, y - 1) != 1) {
			return 0;
		}
	}
	if (goban_get_color(goban, x - 1, y) == color) {
		if (goban_group_libs(goban, x - 1, y) != 1) {
			return 0;
		}
	}

	/* fail (only happens if suicide occurs) */
	return 1;
}

int goban_play(struct goban *goban, int x, int y, int color) {
	int opponent;
	int libs;
	int size;
	int caps;
	int group, other;

	opponent = (color == COL_W) ? COL_B : COL_W;

	/* check move */
	if (goban_check(goban, x, y, color)) {
		return 1;
	}

	/* reduce liberties of and possibly capture opponent */
	caps = 0;
	if (goban_get_color(goban, x, y + 1) == opponent) {
		if (goban_group_libs(goban, x, y + 1) == 1) {
			caps += goban_group_size(goban, x, y + 1);
			_group_clear(goban, goban_get_grpid(goban, x, y + 1));
		}
		else {
			goban->group[goban_get_grpid(goban, x, y + 1)].libs --;
		}
	}
	if (goban_get_color(goban, x + 1, y) == opponent) {
		if (goban_group_libs(goban, x + 1, y) == 1) {
			caps += goban_group_size(goban, x + 1, y);
			_group_clear(goban, goban_get_grpid(goban, x + 1, y));
		}
		else {
			goban->group[goban_get_grpid(goban, x + 1, y)].libs --;
		}
	}
	if (goban_get_color(goban, x, y - 1) == opponent) {
		if (goban_group_libs(goban, x, y - 1) == 1) {
			caps += goban_group_size(goban, x, y - 1);
			_group_clear(goban, goban_get_grpid(goban, x, y - 1));
		}
		else {
			goban->group[goban_get_grpid(goban, x, y - 1)].libs --;
		}
	}
	if (goban_get_color(goban, x - 1, y) == opponent) {
		if (goban_group_libs(goban, x - 1, y) == 1) {
			caps += goban_group_size(goban, x - 1, y);
			_group_clear(goban, goban_get_grpid(goban, x - 1, y));
		}
		else {
			goban->group[goban_get_grpid(goban, x - 1, y)].libs --;
		}
	}

	/* merge with adjacent allies */
	size = 1;
	libs = 0;
	group = _group_new_id(goban);

	if (goban_get_color(goban, x, y + 1) == color) {
		other = goban_get_grpid(goban, x, y + 1);

		if (other != group) {
			libs += goban->group[other].libs - 1;
			size += goban->group[other].size;
			_group_set_id(goban, other, group);
		}
	}
	if (goban_get_color(goban, x + 1, y) == color) {
		other = goban_get_grpid(goban, x + 1, y);

		if (other != group) {
			libs += goban->group[other].libs - 1;
			size += goban->group[other].size;
			_group_set_id(goban, other, group);
		}
	}
	if (goban_get_color(goban, x, y - 1) == color) {
		other = goban_get_grpid(goban, x, y - 1);

		if (other != group) {
			libs += goban->group[other].libs - 1;
			size += goban->group[other].size;
			_group_set_id(goban, other, group);
		}
	}
	if (goban_get_color(goban, x - 1, y) == color) {
		other = goban_get_grpid(goban, x - 1, y);

		if (other != group) {
			libs += goban->group[other].libs - 1;
			size += goban->group[other].size;
			_group_set_id(goban, other, group);
		}
	}

	/* count liberties */
	if (goban_get_color(goban, x, y + 1) == COL_E) {
		if ((goban_get_grpid(goban, x + 0, y + 2) != group) &&
			(goban_get_grpid(goban, x + 1, y + 1) != group) &&
			(goban_get_grpid(goban, x - 1, y + 1) != group)) {
				libs++;
		}
	}

	if (goban_get_color(goban, x + 1, y) == COL_E) {
		if ((goban_get_grpid(goban, x + 1, y + 1) != group) &&
			(goban_get_grpid(goban, x + 2, y + 0) != group) &&
			(goban_get_grpid(goban, x + 1, y - 1) != group)) {
				libs++;
		}
	}
	
	if (goban_get_color(goban, x, y - 1) == COL_E) {
		if ((goban_get_grpid(goban, x + 0, y - 2) != group) &&
			(goban_get_grpid(goban, x + 1, y - 1) != group) &&
			(goban_get_grpid(goban, x - 1, y - 1) != group)) {
				libs++;
		}
	}
	
	if (goban_get_color(goban, x - 1, y) == COL_E) {
		if ((goban_get_grpid(goban, x - 1, y + 1) != group) &&
			(goban_get_grpid(goban, x - 2, y + 0) != group) &&
			(goban_get_grpid(goban, x - 1, y - 1) != group)) {
				libs++;
		}
	}

	/* place piece */
	goban->color[y] |= ((uint64_t) color << (x * 2));
	goban->grpid[y * 19 + x] = group;

	/* update group */
	goban->group[group].libs = libs;
	goban->group[group].size = size;

	/* remember captures and set ko */
	if (caps) {
		goban->ko_group = group;
		if (color == COL_W) {
			goban->w_caps += caps;
		}
		else {
			goban->b_caps += caps;
		}
	}
	else {
		goban->ko_group = 0;
	}

	return 0;
}

static void _group_set_id(struct goban *goban, int grpid_old, int grpid_new) {
	int i;

	for (i = 0; i < 361; i++) {
		if (goban->grpid[i] == grpid_old) {
			goban->grpid[i] = grpid_new;
		}
	}

	_group_del_id(goban, grpid_old);
}

static int _group_new_id(struct goban *goban) {
	int i;

	for (i = 0; i < 256; i++) {
		if ((goban->grpbm[i / 8] & (1 << (i % 8))) == 0) {
			goban->grpbm[i / 8] |= (1 << (i % 8));
			return i;
		}
	}

	return 0;
}

static void _group_del_id(struct goban *goban, int grpid) {	
	goban->grpbm[grpid / 8] &= ~(1 << (grpid % 8));
}

static void _group_clear(struct goban *goban, int grpid) {
	int x, y;
	int group0, group1, group2, group3;

	for (y = 0; y < 19; y++) {
		for (x = 0; x < 19; x++) {
			if (goban->grpid[y * 19 + x] == grpid) {
				goban->grpid[y * 19 + x] = 0;
				goban->color[y] &= ~((uint64_t) 3 << (x * 2));

				group0 = goban_get_grpid(goban, x, y + 1);
				group1 = goban_get_grpid(goban, x + 1, y);
				group2 = goban_get_grpid(goban, x, y - 1);
				group3 = goban_get_grpid(goban, x - 1, y);

				if (group1 == group0) group1 = 0;
				if (group2 == group0 || group2 == group1) group2 = 0;
				if (group3 == group0 || group3 == group2 || group3 == group0) group3 = 0;
				
				if (group0) goban->group[group0].libs++;
				if (group1) goban->group[group1].libs++;
				if (group2) goban->group[group2].libs++;
				if (group3) goban->group[group3].libs++;
			}
		}
	}

	_group_del_id(goban, grpid);
}
