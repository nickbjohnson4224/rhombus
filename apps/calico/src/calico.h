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

#ifndef CALICO_H
#define CALICO_H

#include <stdint.h>

/*
 * 
 * GO BASICS
 *
 */

/* constants for pieces *****************************************************/

#define COL_E	0	// Empty Space
#define COL_B	1	// Black
#define COL_W	2	// White
#define COL_O	3	// Off-Board

/* group representation ******************************************************
 *
 * A group is a connected set of pieces on the goban of the same color. The
 * color of a group can be an empty space, but the goban implementation does
 * not track empty space groups.
 */

struct group {
	uint8_t size;	// number of members
	uint8_t libs;   // number of liberties
};

/* goban representation ******************************************************
 *
 * All the tools needed to simulate the rules of Go. 
 *
 * Notes:
 * Because it is necessary to count the liberties of all groups in order 
 * to determine captures, this implementation keeps track of all groups on the 
 * goban along with their size and liberty count: this is in fact _faster_ 
 * than evaluating captures at every play, so you might as well take advantage 
 * of the metadata about groups for AI purposes. The goban is assumed to be 
 * 19 by 19 intersections.
 */

struct goban {
	uint64_t 		color[19];	// color of each position (2 bits each)
	uint8_t         grpid[361];	// group ID of each position
	struct group	group[256];	// groups
	uint8_t         grpbm[8];	// group allocation bitmap
	int             ko_group;	// group last created
	int				b_caps; 	// number of captures by black
	int				w_caps;		// number of captures by white
};

struct goban *goban_cons (void);
struct goban *goban_clone(struct goban *goban);
int           goban_check(struct goban *goban, int x, int y, int color);
int           goban_play (struct goban *goban, int x, int y, int color);

int           goban_group_size(struct goban *goban, int x, int y);
int           goban_group_libs(struct goban *goban, int x, int y);
int           goban_get_color (struct goban *goban, int x, int y);
int           goban_get_grpid (struct goban *goban, int x, int y);

/*
 *
 * INTERFACE / PROTOCOLS
 *
 */

int print_goban(struct goban *goban);
int read_move(int *x, int *y);

/*
 *
 * CALICO AI
 *
 */

#endif/*CALICO_H*/
