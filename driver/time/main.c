/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
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

#include <ipc.h>
#include <proc.h>
#include <driver.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <natio.h>

#include "time.h"

void time_read(struct packet *packet, uint8_t port, uint32_t caller);

int main() {

	when(PORT_READ, time_read);

	vffile("", "", 1);

	psend(PORT_CHILD, getppid(), NULL);
	_done();

	return 0;
}

void time_read(struct packet *packet, uint8_t port, uint32_t caller) {
	char *data;

	if (!packet) {
		return;
	}

	data = pgetbuf(packet);
	sprintf(data, "%d", get_time());
	psetbuf(&packet, 21);

	psend(PORT_REPLY, caller, packet);
}
