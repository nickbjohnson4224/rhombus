#ifndef FLUX_H
#define FLUX_H

#include <stdint.h>

/***** SYSTEM CALLS *****/
int32_t		_fire(uint32_t pid, uint16_t signal, void *grant, uint32_t flags);
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

#define FIRE_NONE	0x0000
#define FIRE_TAIL	0x0001

#define MMAP_READ	0x001
#define MMAP_WRITE	0x002
#define MMAP_EXEC 	0x004
#define MMAP_FREE	0x008
#define MMAP_FRAME	0x010
#define MMAP_PHYS	0x020
#define MMAP_MOVE	0x040

#define CTRL_NONE		0x00000000
#define CTRL_SCHED		0x00000001
#define CTRL_SIGNAL		0x00000002
#define CTRL_ENTER		0x00000004
#define CTRL_SUPER		0x00000008
#define CTRL_PORTS		0x00000010
#define CTRL_IRQRD		0x00000020
#define CTRL_FLOAT		0x00000040
#define CTRL_RENICE		0x00000080
#define CTRL_CBLOCK		0x00001000
#define CTRL_CCLEAR		0x00002000
#define CTRL_DBLOCK		0x00010000
#define CTRL_DCLEAR		0x00020000
#define CTRL_ASYNC		0x00100000
#define CTRL_MULTI		0x00200000
#define CTRL_QUEUE		0x00400000
#define CTRL_MMCLR		0x00800000

#define CTRL_NICEMASK	0x00000F00
#define CTRL_NICE(n)	(((n) & 0xF) << 8)
#define CTRL_IRQMASK	0xFF000000
#define CTRL_IRQ(n)		(((n) & 0xFF) << 24)

#define SSIG_FAULT	0
#define SSIG_ENTER	1
#define SSIG_PAGE	2
#define SSIG_IRQ	3
#define SSIG_KILL	4
#define SSIG_IMAGE	5
#define SSIG_FLOAT	6
#define SSIG_DEATH	7

#define SIG_READ	16
#define SIG_WRITE	17
#define SIG_INFO	18
#define SIG_CTRL	19
#define SIG_PING	20
#define SIG_ERROR	21
#define SIG_REPLY	32

/***** API FUNCTIONS *****/

void block(void);
void unblock(void);

typedef struct {
	uint32_t task;
	uint32_t resource;
	uint32_t fpos;
} file_t;

size_t read(file_t *fd, char *buffer, size_t count);
size_t write(file_t *fd, char *buffer, size_t count);

#endif
