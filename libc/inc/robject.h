/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#ifndef __RLIBC_ROBJECT_H
#define __RLIBC_ROBJECT_H

#include <rhombus.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*****************************************************************************
 * Rhombus Objects (robjects)
 *
 * Rhombus objects (robjects) are a framework implementing the server side of
 * the Rhombus resource protocols in C. Robjects can be made to represent
 * files, directories, pipes, windows, interfaces, devices, drivers,
 * processes... anything that processes or users might want to interact with.
 *
 * The things contained within this sublibrary are used to send, route, and
 * recieve requests to and from robjects, usually via the low-level message 
 * passing IPC layer.
 *
 * For normal processes, this framework is used only to handle rcall requests
 * to the process head, and generally has only one instantiated robject at
 * index 0.
 */

// defined below
struct robject;

// class from which all others are derived
extern struct robject *robject_class_basic;

// called from libc initialization; sets up robject_class_basic
void __robject_init(void);

// rcall hook format
typedef char *(*rcall_t)(struct robject *self, rp_t src, int argc, char **argv);

/*****************************************************************************
 * Rhombus Object Indexing and Lookup
 */

extern struct robject *robject_root;

void            robject_set(uint32_t index, struct robject *ro);
struct robject *robject_get(uint32_t index);

uint32_t robject_new_index(void);

/*****************************************************************************
 * Rhombus Object Operations
 */

struct robject {
	bool     mutex; // OPT - this should be a readers/writer lock
	bool     driver_mutex;
	uint32_t index; // object index within process; do not modify
	
	// parent interface (for defaulting messages to)
	struct robject *parent;

	// robject fields
	struct s_table *call_table; // table of rcall hooks
	struct s_table *call_stat_table; // table of rcall status levels
	struct s_table *data_table; // table of general data
	struct s_table *subs_table; // table of event subscribers
	struct s_table *open_table; // table of openers and open statuses
	struct s_table *accs_table; // table of user access bitmaps
};

// constructor/destructor
struct robject *robject_cons(uint32_t index, struct robject *parent);
void            robject_free(struct robject *ro);

// various field manipulations
void    robject_set_call(struct robject *ro, const char *call, rcall_t hook, int status);
rcall_t robject_get_call(struct robject *ro, const char *call);
void    robject_set_data(struct robject *ro, const char *field, void *data);
void   *robject_get_data(struct robject *ro, const char *field);

// type system
int robject_is_type(const char *typestr, const char *type);
int robject_check_type(struct robject *ro, const char *type);

// permission system
int  robject_check_access(struct robject *ro, rp_t source, int access);
int  robject_check_status(struct robject *ro, rp_t source, int status);
int  robject_count_status(struct robject *ro, int status);

void robject_set_access  (struct robject *ro, rp_t source, int access);
int  robject_get_access  (struct robject *ro, rp_t source);
void robject_set_default_access(struct robject *ro, int access);

// basic interface
void  robject_event(struct robject *ro, const char *event);
char *robject_call (struct robject *ro, rp_t source, const char *args);
void *robject_data (struct robject *ro, const char *field);

int   robject_open (struct robject *ro, rp_t source, int access);
int   robject_stat (struct robject *ro, rp_t source);
void  robject_close(struct robject *ro, rp_t source);

#endif/*__RLIBC_ROBJECT_H*/
