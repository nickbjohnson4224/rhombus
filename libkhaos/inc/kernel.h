#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

/***** SYSTEM CALLS *****/

/* Interprocess communication */
#define SINT_BLOCK 1
#define SRET_UNBLOCK_SELF 1
#define SRET_UNBLOCK_CALLER 2

int32_t ssnd_call(uint32_t target, uint8_t signal, uint32_t arg0, 
	uint32_t arg1, uint32_t arg2, uint32_t arg3, uint8_t flags);
void sret_call(uint8_t flags);
void sblk_call(uint32_t value);
void sreg_call(uint32_t handler);

/***** ABI 2 SYSTEM CALLS *****/
int32_t		_fire(uint32_t pid, uint32_t flags, uint16_t signal);
void		_drop(void);
uintptr_t	_hand(uintptr_t handler);
uint32_t	_ctrl(uint32_t flags, uint32_t mask);
uint32_t	_info(uint32_t selector);
int32_t		_mmap(uintptr_t addr, uint32_t flags, uint32_t frame);
int32_t		_fork(void);
void		_exit(uint32_t value);

#define fork _fork
#define exit _exit
#define info _info
#define ctrl _ctrl

#define MMAP_READ	0x001
#define MMAP_WRITE	0x002
#define MMAP_EXEC 	0x004
#define MMAP_FREE	0x008
#define MMAP_GRANT	0x010
#define MMAP_PHYS	0x020

#define CTRL_NONE		0x00000000
#define CTRL_SCHED		0x00000001
#define CTRL_SIGNAL		0x00000002
#define CTRL_ENTER		0x00000004
#define CTRL_SUPER		0x00000008
#define CTRL_PORTS		0x00000010
#define CTRL_IRQRD		0x00000020
#define CTRL_FLOAT		0x00000040
#define CTRL_RENICE		0x00000080
#define CTRL_NICEMASK	0x00000F00
#define CTRL_NICE(n)	(((n) & 0xF) << 8)
#define CTRL_IRQMASK	0xFF000000
#define CTRL_IRQ(n)		(((n) & 0xFF) << 24)

#endif
