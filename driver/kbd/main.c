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

#include <driver.h>
#include <proc.h>
#include <stdlib.h>
#include <string.h>
#include <mutex.h>

struct event_recipient {
	struct event_recipient *next;
	struct event_recipient *prev;
	uint64_t rp;
} *event_recipients;

char *kbd_rcall(uint64_t source, struct vfs_obj *file, const char *args) {
	struct event_recipient *r;

	if (!strcmp(args, "register")) {
		mutex_spin(&file->mutex);
		for (r = event_recipients; r; r = r->next) {
			if (r->rp == source) {
				mutex_free(&file->mutex);
				return NULL;
			}
		}

		r = malloc(sizeof(struct event_recipient));
		if (!r) return NULL;
		r->rp = source;
		r->next = event_recipients;
		r->prev = NULL;
		if (r->next) r->next->prev = r;
		event_recipients = r;
		mutex_free(&file->mutex);
		return strdup("T");
	}

	if (!strcmp(args, "deregister")) {
		mutex_spin(&file->mutex);
		for (r = event_recipients; r; r = r->next) {
			if (r->rp == source) {
				if (r->next) r->next->prev = r->prev;
				if (r->prev) r->prev->next = r->next;
				free(r);
				mutex_free(&file->mutex);
				return strdup("T");
			}
		}
		mutex_free(&file->mutex);
		return NULL;
	}

	return NULL;
}

void kbd_send_event(int code) {
	struct event_recipient *r;

	for (r = event_recipients; r; r = r->next) {
		event(r->rp, (uint64_t) code);
	}
}

#define ALT  0x00800000
#define CTRL 0x00800001
#define SHFT 0x00800002
#define SYSR 0x00800003
#define WIN	 0x00800004
#define CAPS 0x00800005
#define INS  0x00800006
#define ENTR 0x00800007
#define UP   0x00800008
#define DOWN 0x00800009
#define LEFT 0x0080000A
#define RGHT 0x0080000B
#define PGUP 0x0080000C
#define PGDN 0x0080000D
#define HOME 0x0080000E
#define END	 0x0080000F
#define ESC  0x00800010

#define F1   0x00800011
#define F2   0x00800012
#define F3   0x00800013
#define F4   0x00800014
#define F5   0x00800015
#define F6   0x00800016
#define F7   0x00800017
#define F8   0x00800018
#define F9   0x00800019
#define F10  0x0080001A
#define F11  0x0080001B
#define F12  0x0080001C

#define NUML 0x0080001D
#define SCRL 0x0080001E
#define DEL  0x0080001F

#define RELEASE 0x00400000

const int keymap[4][128] = {
	{	// lower case, no numlock
		0,
		ESC,  '1',  '2',  '3', '4',  '5', '6',  '7', '8',  '9', '0', '-',  '=', '\b',
		'\t', 'q',  'w',  'e', 'r',  't', 'y',  'u', 'i',  'o', 'p', '[',  ']', '\n', 
		CTRL, 'a',  's',  'd', 'f',  'g', 'h',  'j', 'k',  'l', ';', '\'',  '`', SHFT, 
		'\\', 'z',  'x',  'c', 'v',  'b', 'n',  'm', ',',  '.', '/', SHFT, '*', 
		ALT,  ' ',  CAPS, F1,  F2,   F3,  F4,   F5,  F6,   F7,  F8,  F9,   F10,
		NUML, SCRL, HOME, UP,  PGUP, '-', LEFT, 5,   RGHT, '+', END, DOWN, PGDN, INS, DEL,
		SYSR, 0,    WIN,  F11, F12,  0,   0,    WIN, WIN,
	},
	{	// lower case, numlock
		0,
		ESC,  '1',  '2',  '3', '4', '5', '6', '7', '8', '9', '0', '-',  '=', '\b',
		'\t', 'q',  'w',  'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[',  ']', '\n', 
		CTRL, 'a',  's',  'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',  '`', SHFT, 
		'\\', 'z',  'x',  'c', 'v', 'b', 'n', 'm', ',', '.', '/', SHFT, '*',
		ALT,  ' ',  CAPS, F1,  F2,  F3,  F4,  F5,  F6,  F7,  F8,  F9,   F10,
		NUML, SCRL, 7,    8,   9,   '-', 4,   5,   6,   '+', 1,   2,    3,   0, '.',
		SYSR, 0,    WIN,  F11, F12, 0,   0,   WIN, WIN,
	},
	{	// upper case, no numlock
		0,
		ESC,  '!',  '@',  '#', '$', '%', '^',  '&', '*',  '(', ')', '_',  '+', '\b',
		'\t', 'Q',  'W',  'E', 'R', 'T', 'Y',  'U', 'I',  'O', 'P', '{',  '}', '\n', 
		CTRL, 'A',  'S',  'D', 'F', 'G', 'H',  'J', 'K',  'L', ':', '"',  '~', SHFT, 
		'\\', 'Z',  'X',  'C', 'V', 'B', 'N',  'M', '<',  '>', '?', SHFT, '*', 
		ALT,  ' ',  CAPS, F1,  F2,   F3,  F4,   F5,  F6,   F7,  F8,  F9,   F10,
		NUML, SCRL, HOME, UP,  PGUP, '-', LEFT, 5,   RGHT, '+', END, DOWN, PGDN, INS, DEL,
		SYSR, 0,    WIN,  F11, F12,  0,   0,    WIN, WIN,
	},
	{	// upper case, numlock
		0,
		ESC,  '!',  '@',  '#', '$', '%', '^',  '&', '*',  '(', ')', '_',  '+', '\b',
		'\t', 'Q',  'W',  'E', 'R', 'T', 'Y',  'U', 'I',  'O', 'P', '{',  '}', '\n', 
		CTRL, 'A',  'S',  'D', 'F', 'G', 'H',  'J', 'K',  'L', ':', '"',  '~', SHFT, 
		'\\', 'Z',  'X',  'C', 'V', 'B', 'N',  'M', '<',  '>', '?', SHFT, '*', 
		ALT,  ' ',  CAPS, F1,  F2,  F3,  F4,  F5,  F6,  F7,  F8,  F9,   F10,
		NUML, SCRL, 7,    8,   9,   '-', 4,   5,   6,   '+', 1,   2,    3,   0, '.',
		SYSR, 0,    WIN,  F11, F12, 0,   0,   WIN, WIN,
	}
};

void kbd_irq(struct msg *msg) {
	uint8_t scan;
	int code;

	static bool shift = false;
	static bool caps  = false;
	static bool numlk = false;

	scan = inb(0x60);

	if (scan == 0xE0) {
		return;
	}

	code = keymap[((shift ^ caps) ? 2 : 0) | ((numlk) ? 1 : 0)][scan & ~0x80];
	if (scan & 0x80) {
		switch (code) {
		case SHFT: shift = false; break;
		case CAPS: caps  = false; break;
		case NUML: numlk = false; break;
		default:
//			printf("release %i %c\n", code, code);
			kbd_send_event(code | RELEASE);
		}
	}
	else {
		switch (code) {
		case SHFT: shift = true; break;
		case CAPS: caps  = true; break;
		case NUML: numlk = true; break;
		default:
//			printf("press %i %c\n", code, code);
			kbd_send_event(code);
		}
	}
}

int main(int argc, char **argv) {
	struct vfs_obj *root;
	
	root = calloc(sizeof(struct vfs_obj), 1);
	root->type = 0;
	root->size = 0;
	root->acl = acl_set_default(root->acl, 0);
	vfs_set_index(0, root);

	di_wrap_irq  (1, kbd_irq);
	di_wrap_rcall(kbd_rcall);
	vfs_wrap_init();

	io_link("/dev/kbd", RP_CONS(getpid(), 0));

	msendb(RP_CONS(getppid(), 0), PORT_CHILD);
	_done();
	
	return 0;
}
