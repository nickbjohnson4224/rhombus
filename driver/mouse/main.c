/*
 * Copyright (C) 2011 Jaagup Repän <jrepan@gmail.com>
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

struct register_t
{
	uint64_t rp;
	struct register_t *next;
};

struct register_t *regs;

void wait_signal() {
	while (inb(0x64) & 0x2);
}

void command(uint8_t byte) {
	wait_signal();
	outb(0x64, 0xd4);
	wait_signal();
	outb(0x60, byte);
	while (inb(0x60) != 0xfa); // ACK
}

char *mouse_rcall(uint64_t source, struct vfs_obj *file, const char *args) {
	struct register_t *reg, *prev = NULL;

	if (strcmp(args, "register") == 0) {
		for (reg = regs; reg; reg = reg->next) {
			if (reg->rp == source) {
				// avoid duplicates
				return strdup("");
			}
		}

		reg = malloc(sizeof(struct register_t));
		if (!reg) {
			return strdup("");
		}
		reg->rp = source;
		reg->next = regs;
		regs = reg;
		return strdup("T");
	}

	if (strcmp(args, "deregister") == 0) {
		for (reg = regs; reg; reg = reg->next) {
			if (reg->rp == source) {
				if (prev) {
					prev->next = reg->next;
				}
				else {
					regs->next = reg->next;
				}
				free(reg);
				return strdup("T");
			}
			prev = reg;
		}
		return strdup("");
	}

	return NULL;
}

int main()
{
	struct vfs_obj *root;
	struct register_t *reg;
	uint8_t bytes[3];
	size_t curbyte = 0;
	bool first;
	int buttons = 0, prevbuttons = 0;
	int clicked, released;
	int16_t dx = 0, dy = 0;

	command(0xa8);  // enable aux. PS2
	command(0xf6);  // load default config
	command(0xf4);  // enable mouse
	command(0xf3);  // set sample rate:
	outb(0x60, 10); //    10 samples per second

	root        = calloc(sizeof(struct vfs_obj), 1);
	root->type  = RP_TYPE_FILE;
	root->size  = 0;
	root->acl   = acl_set_default(root->acl, PERM_READ | PERM_WRITE);
	vfs_set_index(0, root);

	di_wrap_rcall(mouse_rcall);
	vfs_wrap_init();

	msendb(RP_CONS(getppid(), 0), PORT_CHILD);

	while (1) {
		first = true;
		while ((inb(0x64) & 0x21) != 0x21) {
			if (first) {
				clicked  =  buttons & ~prevbuttons;
				released = ~buttons &  prevbuttons;
				if (dx || dy) {
					for (reg = regs; reg; reg = reg->next) {
						event(reg->rp, 0x1LL << 62 | (dx << 16) & 0xffff0000 | dy & 0xffff);
					}
				}
				if (clicked) {
					for (reg = regs; reg; reg = reg->next) {
						event(reg->rp, 0x2LL << 62 | clicked);
					}
				}
				if (released) {
					for (reg = regs; reg; reg = reg->next) {
						event(reg->rp, 0x3LL << 62 | released);
					}
				}
				dx = dy = 0;
				prevbuttons = buttons;
			}
			first = false;
		}

		bytes[curbyte++] = inb(0x60);

		if (curbyte == 1) {
			buttons = bytes[0] & 0x7;
		}
		if (curbyte == 3) {
			curbyte = 0;
			dx += bytes[1] - ((bytes[0] & 0x10) ? 256 : 0);
			dy -= bytes[2] - ((bytes[0] & 0x20) ? 256 : 0);
		}
	}
	
	return 0;
}
