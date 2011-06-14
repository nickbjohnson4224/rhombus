/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail dot com>
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

#include <stdlib.h>
#include <string.h>
#include <driver.h>
#include <natio.h>
#include <mutex.h>
#include <proc.h>
#include <ipc.h>

bool mutex[MAX_PID];
struct event_list *event_list[MAX_PID];

char *ptrack_register(uint64_t source, uint32_t index, int argc, char **argv) {
	
	if (index > MAX_PID) return strdup("! nfound");

	mutex_spin(&mutex[index]);
	event_list[index] = event_list_add(event_list[index], source);
	mutex_free(&mutex[index]);

	return strdup("T");
}

char *ptrack_deregister(uint64_t source, uint32_t index, int argc, char **argv) {
	
	if (index > MAX_PID) return strdup("! nfound");

	mutex_spin(&mutex[index]);
	event_list[index] = event_list_del(event_list[index], source);
	mutex_free(&mutex[index]);

	return strdup("T");
}

void ptrack_irq(struct msg *msg) {
	uint32_t index = RP_PID(msg->source);
	char *event_str;

	if (msg->source != 0) return;
	if (index > MAX_PID) return;

	event_str = saprintf("ptrack exit %d", index);

	mutex_spin(&mutex[index]);
	eventl(event_list[index], event_str);
	event_list[index] = NULL;
	mutex_free(&mutex[index]);
}

int main(int argc, char **argv) {
	
	// register process tracker "IRQ"
	di_wrap_irq(255, ptrack_irq);

	// register rcall handlers
	rcall_set("register", ptrack_register);
	rcall_set("deregister", ptrack_deregister);

	fs_plink("/sys/ptrack", RP_CONS(getpid(), 0), NULL);

	msendb(RP_CONS(getppid(), 0), PORT_CHILD);
	_done();

	return 0;
}
