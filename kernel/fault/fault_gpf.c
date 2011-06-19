/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <interrupt.h>
#include <thread.h>
#include <debug.h>
#include <space.h>
#include <cpu.h>
#include <ports.h>

int vm86_monitor_gpf(struct thread *);

/*****************************************************************************
 * fault_gpf
 *
 * Fault handler for general protection faults. This is distinct from the 
 * generic fault because it is used to implement the virtual monitor on vm86 
 * tasks.
 */

struct thread *fault_gpf(struct thread *image) {

	/* if vm86 is enabled, try to handle */
	if (image->vm86_active) {
		switch (vm86_monitor_gpf(image)) {
		case 0: 
			return image;
		case 1: 
			image->vm86_active = 0;
			image->eip = image->vm86_saved_eip;
			image->useresp = image->vm86_saved_esp;
			image->ds  = 0x23;
			image->ss  = 0x23;
			image->cs  = 0x1B;
			image->eflags = image->vm86_saved_eflags;
			image->eax = 0;
			set_int_stack(&image->vm86_es);
			return image;
		}
	}
	
	/* If in kernelspace, panic */
	if ((image->cs & 0x3) == 0) {
		debug_printf("EIP:%x NUM:%d ERR:%x EFLAGS:%x\n", image->eip, image->num, image->err, image->eflags);
		debug_panic("general protection fault");
	}

	/* if all else fails, send SIGILL to process */
	process_freeze(image->proc);
	return thread_send(image, image->proc->pid, PORT_ILL, NULL);
}

int vm86_monitor_gpf(struct thread *image) {
	uint16_t *stack;
	uint16_t *ivt;
	uint8_t  *ip;
	bool     o32, a32;

	ip    = (void*) (image->eip + (image->cs << 4));
	ivt   = (void*) 0;
	stack = (void*) (image->useresp + (image->ss << 4));
	o32 = a32 = false;

	while (1) switch (*ip) {
	case 0x66: /* O32 */
		o32 = true;
		ip++;
		image->eip = (uint16_t) (image->eip + 1);
		break;
	case 0x67: /* A32 */
		a32 = true;
		ip++;
		image->eip = (uint16_t) (image->eip + 1);
		break;
	case 0x9C: /* PUSHF */
		if (o32) {
			image->useresp = (uint16_t) (image->useresp - 4);
			stack -= 2;
			stack[0] = image->eflags;
			stack[1] = image->eflags >> 16;
		}
		else {
			image->useresp = (uint16_t) (image->useresp - 2);
			stack--;
			stack[0] = image->eflags;
		}

		if (image->vm86_if) stack[0] |= 0x200;
		else stack[0] &= ~0x200;

		image->eip = (uint16_t) (image->eip + 1);
		return 0;
	case 0x9D: /* POPF */
		image->eflags = 0x20200 | (stack[0] & 0x0DFF);
		image->vm86_if = (stack[0] & 0x200) ? 1 : 0;

		if (o32) image->useresp = (uint16_t) (image->useresp + 4);
		else     image->useresp = (uint16_t) (image->useresp + 2);

		image->eip = (uint16_t) (image->eip + 1);
		return 0;
	case 0xCD: /* INT */
		if (ip[1] == 0x30) {
			return 1;
		}

		stack -= 3;
		image->useresp = (uint16_t) (image->useresp - 6);
		stack[0] = image->eip + 2;
		stack[1] = image->cs;
		stack[2] = image->eflags;

		if (image->vm86_if) stack[2] |= 0x200;
		else stack[2] &= ~0x200;
	
		image->eip = ivt[(ip[1] * 2) + 0];
		image->cs  = ivt[(ip[1] * 2) + 1];

		return 0;
	case 0xCF: /* IRET */
		image->eip     = stack[0];
		image->cs      = stack[1];
		image->eflags  = stack[2] | 0x20200;
		image->vm86_if = (stack[2] & 0x200) ? 1 : 0;
		image->useresp = (uint16_t) (image->useresp + 6);
		return 0;
	case 0xEC: /* INB */
		image->eax &= ~0xff;
		image->eax |= inb(image->edx & 0xffff);
		image->eip = (uint16_t) (image->eip + 1);
		return 0;
	case 0xED: /* INW */
		if (o32) {
			image->eax = ind(image->edx & 0xffff);
		}
		else {
			image->eax &= ~0xffff;
			image->eax |= inw(image->edx & 0xffff);
		}
		image->eip = (uint16_t) (image->eip + 1);
		return 0;
	case 0xEE: /* OUTB */
		outb(image->edx & 0xffff, image->eax & 0xff);
		image->eip = (uint16_t) (image->eip + 1);
		return 0;
	case 0xEF: /* OUTW */
		if (o32) {
			outd(image->edx & 0xffff, image->eax);
		}
		else {
			outw(image->edx & 0xffff, image->eax & 0xffff);
		}
		image->eip = (uint16_t) (image->eip + 1);
		return 0;
	case 0xFA: /* CLI */
		image->vm86_if = 0;
		image->eip = (uint16_t) (image->eip + 1);
		return 0;
	case 0xFB: /* STI */
		image->vm86_if = 1;
		image->eip = (uint16_t) (image->eip + 1);
		return 0;
	default:
		debug_printf("vm86 unhandled opcode: %x at %x\n", *ip, ip);
		debug_panic("illegal instruction");

		return 2;
	}
}
