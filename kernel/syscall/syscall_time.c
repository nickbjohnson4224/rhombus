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

#include <interrupt.h>
#include <timer.h>

/*****************************************************************************
 * syscall_time (int 0x4d)
 *
 * ECX: selector
 *
 * Returns various information about kernel time in the register pair
 * EAX:EDX. There should be no problems ever with overflow on this. Even with
 * hundreds of processors, the kernel tick should reset every few dozen
 * MILLION YEARS. However, this time is not meant to be super-accurate, and
 * may be inaccurate to multiple seconds per day.
 *
 * If selector is 0, the time is all ticks since kernel bootup.
 * If selector is 1, the time is ticks on the current cpu since bootup.
 * If selector is 2, the time is ticks on the current process.
 * If selector is 3, the time is ticks on the current thread.
 * If selector is 4, the value is nanoseconds per tick.
 */

struct thread *syscall_time(struct thread *image) {
	uint64_t tick;

	switch (image->ecx) {
	case 0: case 1:
		tick = timer_get_tick();
		break;
	case 2:
		tick = image->proc->tick;
		break;
	case 3:
		tick = image->tick;
		break;
	case 4:
		tick = 3906250; // 1000000000 / 256
		break;
	}
	
	image->eax = (tick >> 0);
	image->edx = (tick >> 32);

	return image;
}
