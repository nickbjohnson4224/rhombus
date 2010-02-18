#ifndef FLUX_H
#define FLUX_H

#include <config.h>
#include <arch.h>

/***** SYSTEM CALLS *****/

int32_t		_fire(uint32_t pid, uint16_t signal, void *grant, uint32_t flags);
void		_drop(void);
uintptr_t	_hand(uintptr_t handler);
uint32_t	_ctrl(uint32_t flags, uint32_t mask, uint32_t space);
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

/***** API FUNCTIONS *****/

/*** Process Control ***/

#define fork _fork		/* Spawn new process */
#define exit _exit		/* Exit current process */
#define info _info		/* Get process information */
#define ctrl _ctrl		/* Set process information */

void block(bool v);		/* (Dis)allow scheduling */
void sleep(void);		/* Relinquish timeslice */

/*** Memory Management ***/

#define MMAP_PAGESIZE PAGESZ

#define PROT_NONE 0
#define PROT_READ MMAP_READ
#define PROT_WRITE MMAP_WRITE
#define PROT_EXEC MMAP_EXEC

int mmap(void *addr, size_t length, int prot);
int umap(void *addr, size_t length);
int emap(void *addr, uint32_t frame, int prot);
uintptr_t phys(void *addr);

/*** Flux Standard Request Protocol ***/

#define REQSZ (PAGESZ - 512)
#define STDOFF 512

/* Request header */
typedef struct request {
	uint32_t checksum;			/* Checksum (bit parity) */
	uint32_t resource;			/* Resource ID */
	uint16_t datasize;			/* Size of request data */
	uint16_t transid;			/* Transaction ID */
	uint16_t dataoff;			/* Offset of request data */
	uint16_t format;			/* Header format */
	uint64_t fileoff;			/* File offset */
	uint64_t fileoff_ext;		/* Extended file offset */
	uint8_t  reqdata[];			/* Request data area */
} __attribute__ ((packed)) req_t;

#define REQ_READ  0
#define REQ_WRITE 1
#define REQ_ERROR 2

req_t *ralloc(void);			/* Allocate request header and buffer */
void   rfree (req_t *r);		/* Free request header and buffer */

req_t *req_cksum(req_t *r);		/* Checksum request */
bool   req_check(req_t *r);		/* Check request for validity */

bool      req_setbuf(req_t *r, uint16_t offset, uint16_t size);
uint8_t  *req_getbuf(req_t *r);

/*** Flux Standard Metadata Space ***/

typedef struct meta {
	uint32_t resource;			/* Resource ID */
	uint32_t type;				/* Resource Type */
	uint8_t resv0[24];

	uint64_t wrtetime;			/* Last data modification */
	uint64_t readtime;			/* Last data access */
	uint64_t ctrltime;			/* Last metadata modification */
	uint64_t infotime;			/* Last metadata access */

	uint32_t flags;				/* Various flags */
	char filename[64];			/* File name */
	uint64_t size;				/* File size */
	uint64_t size_ext;			/* Extended file size */
	uint8_t resv2[44];
} __attribute__ ((packed)) meta_t;

/*** Flux Signals ***/

#define MAXSIGNAL	32

void sigblock(bool v, uint8_t signal);	/* (Dis)allow signals */

int  fire(uint32_t target, uint8_t signal, req_t *req);
void tail(uint32_t target, uint8_t signal, req_t *req);

typedef void (*sig_handler_t) (uint32_t caller, req_t *req);
void sigregister(uint16_t signal, sig_handler_t handler);

void   sighold(uint16_t signal);			/* Hold signal */
void   sigfree(uint16_t signal);			/* Stop holding signal */
req_t *sigpull(uint16_t signal);			/* Get held signal request */
void   sigpush(uint16_t signal, req_t *r);	/* Re-hold signal request */

#define SSIG_FAULT	0x00
#define SSIG_ENTER	0x01
#define SSIG_PAGE	0x02
#define SSIG_IRQ	0x03
#define SSIG_KILL	0x04
#define SSIG_IMAGE	0x05
#define SSIG_FLOAT	0x06
#define SSIG_DEATH	0x07

#define SIG_READ	0x10
#define SIG_WRITE	0x11
#define SIG_INFO	0x12
#define SIG_CTRL	0x13
#define SIG_REPLY	0x1F

#define VSIG_ALL	0xFF
#define VSIG_REQ	0xFE

/***** Driver API *****/

/*** Port Access ***/
uint8_t  inb(uint16_t port);
uint16_t inw(uint16_t port);
uint32_t ind(uint16_t port);

void outb(uint16_t port, uint8_t value);
void outw(uint16_t port, uint16_t value);
void outd(uint16_t port, uint32_t value);

void iodelay(uint32_t usec);

/*** Device Descriptor ***/

typedef struct device {
	uint8_t type;
	uint8_t bus;
	uint8_t slot;
	uint8_t sub;
} device_t;

#define DEV_TYPE_NATIVE	0
#define DEV_TYPE_PCI	1
#define DEV_TYPE_FREE	2

/*** Driver Interface Structure ***/

struct driver_interface {
	void (*init) (device_t dev);	/* Initialize driver on device */
	void (*halt) (void);			/* De-initialize device */
	void (*work) (void);			/* Do background work */
	size_t jobs;					/* Number of background jobs to complete */
};

/*** IRQ Redirection ***/

void rirq(uint8_t irq);

#endif
