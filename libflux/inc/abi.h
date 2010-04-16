/* Copyright 2010 Nick Johnson */

#ifndef ABI_H
#define ABI_H

#include <flux/arch.h>

/***** SYSTEM CALLS *****/

uint32_t	_fire(uint32_t target, uint16_t signal, void *grant, bool drop);
void		_drop(void);
uint32_t    _sctl(uint32_t action, uint32_t signal, uint32_t value);
uintptr_t   _mail(uint32_t signal, uint32_t source, uint32_t insert);

uint32_t	_pctl(uint32_t flags, uint32_t mask);

uint32_t	_info(uint32_t selector);
int32_t		_mmap(uintptr_t addr, uint32_t flags, uint32_t frame);

int32_t		_fork(void);
void		_exit(uint32_t value);

#define FIRE_NONE	0x0000
#define FIRE_TAIL	0x0001

#define MMAP_READ	0x001
#define MMAP_WRITE	0x002
#define MMAP_EXEC 	0x004
#define MMAP_FREE	0x008
#define MMAP_FRAME	0x010
#define MMAP_PHYS	0x020
#define MMAP_MOVE	0x040

#define SCTL_POLICY		0
#define SCTL_HANDLE		1

#define CTRL_PSPACE 	0
#define CTRL_SSPACE 	1

#define CTRL_NONE		0x00000000
#define CTRL_BLOCK		0x00000001
#define CTRL_CLEAR		0x00000002
#define CTRL_ENTER		0x00000004
#define CTRL_SUPER		0x00000008
#define CTRL_PORTS		0x00000010
#define CTRL_IRQRD		0x00000020
#define CTRL_FLOAT		0x00000040
#define CTRL_RENICE		0x00000080
#define CTRL_CBLOCK		0x00001000
#define CTRL_CCLEAR		0x00002000
#define CTRL_IRQST		0x00004000
#define CTRL_DBLOCK		0x00010000
#define CTRL_DCLEAR		0x00020000
#define CTRL_ASYNC		0x00100000
#define CTRL_MULTI		0x00200000
#define CTRL_QUEUE		0x00400000
#define CTRL_MMCLR		0x00800000

#define INFO_GPID		0x00000001
#define INFO_PPID		0x00000002
#define INFO_TICK		0x00000003
#define INFO_ABIVER		0x00000004
#define INFO_LIMIT		0x00000005
#define INFO_CTRL		0x00000006
#define INFO_MMAP		0x00000007
#define INFO_SFLAGS		0x00000008

#define CTRL_NICEMASK	0x00000F00
#define CTRL_NICE(n)	(((n) & 0xF) << 8)
#define CTRL_IRQMASK	0xFF000000
#define CTRL_IRQ(n)		(((n) & 0xFF) << 24)

#endif/*ABI_H*/
