#ifndef FLUX_SIGNAL_H
#define FLUX_SIGNAL_H

#include <flux/request.h>
#include <flux/arch.h>

#define MAXSIGNAL	32

#define SSIG_FAULT	0x00
#define SSIG_ENTER	0x01
#define SSIG_PAGE	0x02
#define SSIG_IRQ	0x03
#define SSIG_KILL	0x04
#define SSIG_IMAGE	0x05
#define SSIG_FLOAT	0x06
#define SSIG_DEATH	0x07

/* General signals */
#define SIG_READ	0x10	/* Read data */
#define SIG_WRITE	0x11	/* Write data */
#define SIG_INFO	0x12	/* Read metadata */
#define SIG_CTRL	0x13	/* Write metadata */
#define SIG_REPLY	0x1F	/* Reply from request */

/* Specific signals */
#define SIG_DEV_NEW		0x14	/* Create file */
#define SIG_DEV_DEL		0x15	/* Delete file */
#define SIG_DEV_LOCK	0x16	/* Lock file */

#define SIG_VFS_ADD 	0x14	/* Add inode */
#define SIG_VFS_DEL 	0x15	/* Delete inode */
#define SIG_VFS_FIND	0x16	/* Find inode */

#define POLICY_ABORT 0
#define POLICY_QUEUE 1
#define POLICY_EVENT 2

int fire(uint32_t target, uint32_t signal, req_t *req);
int tail(uint32_t target, uint32_t signal, req_t *req);  

uint32_t signal_policy(uint32_t signal, uint32_t policy);
req_t   *signal_recv  (uint32_t signal);
req_t   *signal_recvs (uint32_t signal, uint32_t source);
req_t   *signal_wait  (uint32_t signal, bool sleep);
req_t   *signal_waits (uint32_t signal, uint32_t source, bool sleep);

typedef void (*sig_handler_t) (uint32_t caller, req_t *req);
void signal_register(uint32_t signal, sig_handler_t handler);

#endif/*FLUX_SIGNAL_H*/
