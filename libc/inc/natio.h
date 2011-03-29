/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#ifndef NATIO_H
#define NATIO_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <path.h>
#include <ipc.h>

/* core I/O *****************************************************************/

#define RP_CONS(pid, idx) ((((uint64_t) (pid)) << 32) | (uint64_t) (idx))
#define RP_PID(rp) ((rp) >> 32)
#define RP_INDEX(rp) ((rp) & 0xFFFFFFFF)
#define RP_NULL ((uint64_t) 0)

#define RP_TYPE_FILE 	0x01
#define RP_TYPE_DIR		0x02

size_t   read (uint64_t rp, void *buf, size_t size, uint64_t offset);
size_t   write(uint64_t rp, void *buf, size_t size, uint64_t offset);
int      sync (uint64_t rp);
int      reset(uint64_t rp);
int      share(uint64_t rp, void *buf, size_t size, uint64_t offset, int prot);
char    *rcall(uint64_t rp, const char *args);
int      event(uint64_t rp, uint64_t value);

char    *rcallf(uint64_t rp, const char *fmt, ...);

/* I/O handling *************************************************************/

typedef void (*event_handler_t)(uint64_t source, uint64_t value);
int      event_register(uint64_t source, event_handler_t handler);

/* filesystem operations ****************************************************/

uint64_t io_find(const char *name);
uint64_t io_cons(const char *name, int type);
int      io_remv(const char *name);
int      io_link(const char *name, uint64_t rp);

extern uint64_t fs_root;

uint64_t fs_find  (uint64_t root, const char *path);
uint64_t fs_lfind (uint64_t root, const char *path);
uint64_t fs_cons  (uint64_t dir, const char *name, int type);
uint64_t fs_move  (uint64_t dir, const char *name, uint64_t file);
char    *fs_list  (uint64_t dir, int entry);
int      fs_link  (uint64_t link, uint64_t fobj);
uint64_t fs_size  (uint64_t file);
int      fs_remove(uint64_t rp);
int      fs_type  (uint64_t rp);

uint8_t  fs_perm  (uint64_t rp, uint32_t user);
int      fs_auth  (uint64_t rp, uint32_t user, uint8_t perm);

#define PERM_READ	0x01
#define PERM_WRITE	0x02
#define PERM_ALTER	0x04

#endif/*NATIO_H*/
