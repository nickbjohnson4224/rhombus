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

#ifndef __RLIBC_RHOMBUS_H
#define __RLIBC_RHOMBUS_H

#include <stdint.h>
#include <rho/proc.h>
#include <rho/types.h>

/*****************************************************************************
 * Resource pointers
 *
 * This stuff right here, this is important. Resource pointers are the way
 * that Rhombus indentifies pretty much everything on the system. Every file,
 * directory, link, thread, process, etc. can be uniquely identified by a 
 * resource pointer, which, because it is an integer type, is easy to pass to 
 * and from functions. Resource pointers also have canonical string 
 * representations which are used in the rcall and event interfaces, and are 
 * human-readable.
 *
 * The string representations have the following format:
 *
 *   @p.i
 *
 * Where p is the PID of the pointer and i is the index, both in decimal.
 * For example, if the PID is 42 and the index is 123, the string 
 * representation would be "@42.123".
 *
 * There are also some macros used to portably interface with robject
 * pointer contents:
 *
 *   RP_CONS(pid, index) - construct a rp with the given PID and index.
 *   RP_PID(rp)          - evaluate to the PID of the rp.
 *   RP_INDEX(rp)        - evaluate to the index of the rp.
 *   RP_HEAD(rp)         - evaluate to the "head" of a pointer's process
 *   RP_NULL             - universally represents no robject; false-like
 *
 *   RP_CURRENT_THREAD   - the resource pointer of the current thread
 *   RP_CURRENT_PROC     - the resource pointer of the whole process
 *
 * The way that the bits are arranged in a resource pointer makes it so that
 * if an rp_t is cast to a PID, it evaluates to the PID of the process owning
 * the resource, and so if a PID is cast to an rp_t, it evaluates to the
 * resource pointer pointing to that process's head.
 *
 * The "head" of a process is a resource that represents both the main thread 
 * of that process and the process itself. It may be used as both a resource
 * or a thread.
 */

#define RP_CONS(pid, idx) ((((uint64_t) (idx)) << 32) | (uint64_t) (pid))
#define RP_INDEX(rp)      ((uint32_t) ((rp) >> 32))
#define RP_PID(rp)        ((uint32_t) ((rp) & 0xFFFFFFFF))
#define RP_HEAD(rp)       ((uint64_t) ((rp) & 0xFFFFFFFF))
#define RP_NULL           ((uint64_t) 0)

#define RP_CURRENT_THREAD RP_CONS(getpid(), -gettid())
#define RP_CURRENT_PROC   RP_CONS(getpid(), 0)

// convert resource pointer to string
char *rtoa(rp_t rp);

// convert string to resource pointer
rp_t ator(const char *str);

/*****************************************************************************
 * File Descriptors
 */

int  fd_alloc(void);
int  fd_set    (int fd, rp_t rp, int mode);
int  fd_pullkey(int fd);
rk_t fd_getkey (int fd, int action);
int  fd_mode   (int fd);
rp_t fd_rp     (int fd);
int  fd_free   (int fd);

int      fd_seek(int fd, uint64_t pos);
uint64_t fd_pos (int fd);

int ropen(int fd, rp_t rp, int mode);
int close(int fd);
int dup  (int fd);
int dup2 (int fd, int newfd);

/*****************************************************************************
 * Resource Action Classes
 *
 * Resources have a multitude of different actions that can be performed on
 * them with both the builtin I/O messages (read, write, mmap, sync, reset) 
 * and with the open-ended rcall messages. Different drivers and resources 
 * within those drivers may expose different sets of actions. To impose some 
 * order on this, actions are divided into "action classes". Whether or not a
 * process can perform an action is dependent on that action's class.
 *
 * The eight action classes are as follows:
 *
 * 0 - AC_NULL
 * 
 *   The default action class. Actions of class AC_NULL are not restricted by
 *   the native access control system, although they may implement their own
 *   based on source and key information.
 *
 *   Example actions: find, ping, name, stat, get-key, get-access, get-ac
 *
 * 1 - AC_READ
 *
 *   The action class for actions that read data from a resource. If the data
 *   is not potentially sensitive (like whether the resource exists or not)
 *   then the AC_NULL class would be used instead.
 *
 *   Example actions: read, size, list, get-link
 *
 * 2 - AC_WRITE
 *
 *   The action class for actions that write data to a resource.
 *
 *   Example actions: write, sync, reset, finish, set-link
 *
 * 3 - AC_ALTER
 *
 *   The action class for actions that alter the directory structure or the
 *   existence of resources, but not for those that change the permissions of
 *   existing resources.
 *
 *   Example actions: create, delete, link, unlink, 
 *
 * 4 - AC_ADMIN
 *
 *   The action class for actions that modify the permissions of existing
 *   resources.
 *
 *   Example actions: set-access
 *
 * 5 - AC_EVENT
 *
 *   The action class for actions that change the event subscriber list of a
 *   resource.
 *
 *   Example actions: listen, un-listen
 *
 * 6 - AC_LOCK
 *
 *   The action class for actions that create both advisory and mandatory read 
 *   and write locks on a resource.
 *
 *   Example actions: lock, unlock
 *
 * 7 - AC_ROOT
 *
 *   The action class for administrative actions pertaining to the driver
 *   itself. Generally very driver-specific.
 *
 *   Example actions: power-down, hard-reset
 */

#define AC_NULL  0
#define AC_READ  1
#define AC_WRITE 2
#define AC_ALTER 3
#define AC_ADMIN 4
#define AC_EVENT 5
#define AC_LOCK  6
#define AC_ROOT  7

/*****************************************************************************
 * Resource Action Keys
 *
 * Every resource has a set of "action keys", one for each action class that
 * may be performed on it. These action keys are used to authenticate actions
 * performed on a resource.
 */

rk_t rp_getkey (rp_t rp, int action);
int  rp_getkeys(rp_t rp, rk_t keys[8]);

/*****************************************************************************
 * Resource Access Control Lists
 *
 * Every resource has a set of access bitmaps that determine which processes
 * and users can perform which actions on that resource. 
 *
 * The following flags correspond to bits that may be set in the access
 * bitmap. Permissions can be assigned on a per-user basis.
 *
 * ACCS_READ  - 0x2
 *
 * This flag allows read access. For directories and links, this means 
 * finding and listing. For files, this means reading file contents.
 *
 * ACCS_WRITE - 0x4
 *
 * This flag allows write access. For directories, this means the creation and
 * deletion of hard links. For files, this means writing, clearing, and 
 * deleting files/file contents, as well as requesting file synchronization.
 * 
 * ACCS_ALTER - 0x8
 *
 * This flag allows the access bitmap to be modified. Some drivers simply
 * do not allow certain operations (usually writing, if the filesystem is
 * read-only) and this does not ensure that the permission bitmap will 
 * actually be modified as specified.
 *
 * ACCS_EVENT - 0x10
 *
 * This flag allows events to be listened to. Some resources simply do not
 * emit events.
 */

#define ACCS_READ  (1 << AC_READ)
#define ACCS_WRITE (1 << AC_WRITE)
#define ACCS_ADMIN (1 << AC_ADMIN)
#define ACCS_EVENT (1 << AC_EVENT)

int rp_access(rp_t rp, uint32_t user);
int rp_admin (rp_t rp, uint32_t user, int access);

/*****************************************************************************
 * High Level Message Passing (rcall)
 *
 * Resources can accept a bunch of I/O specific messages (see natio.h), but
 * more commonly use a generic text-based message protocol called rcall.
 *
 * This protocol is very simple. The only argument is an ASCII string, and the
 * only return value is an ASCII string. Within the argument, there is some
 * structure, however. The argument string is interpreted as a sequence of
 * space-separated tokens (much like the arguments to a command line utility),
 * with the first token being the name of the function to be called.
 */

// perform an rcall
char *rcall (rp_t rp, rk_t key, const char *fmt, ...);
char *frcall(int fd, int aclass, const char *fmt, ...);

// root rcall hook format
typedef char *(*rcall_hook_t)(rp_t src, int argc, char **argv);

// set the rcall hook for a given rcall function
int    rcall_hook(const char *func, rcall_hook_t hook);
char  *rcall_call(rp_t source, const char *args);
void __rcall_init(void);

/**************************************************************************** 
 * High Level Event System (event)
 *
 * The event protocol is an asyncronous, broadcasting parallel of the rcall 
 * protocol. Only a single ASCII string is sent as event data, and events are 
 * sent from robjects to processes. Each resource maintains a list of "event 
 * subscribers", to which messages are sent if an event is to be sent from 
 * that resource.
 *
 * Instead of a method name like in rcall, event uses the first token of the
 * argument string as an event type, which is used to route it. Event types
 * should be used to group similar events together (like keypress events, or
 * mouse movement events, or window events.)
 */

// event hook format
typedef void (*event_t)(rp_t src, int argc, char **argv);

// set the event hook for the given event type
int event_hook(const char *type, event_t hook);

// send an event
int event(rp_t rp, const char *value);
int fevent(int fd, const char *value);

/*****************************************************************************
 * Resource Type System
 *
 * Different resources implement represent different types of things and
 * implement different rcall interfaces, and therefore it is convenient to
 * be able to determine whether a resource is of a certain type. Resource
 * types are represented as space-separaed string lists of type names, 
 * effectively sets.
 *
 * To determine whether a resource <R> implements a type <type>, use 
 * rp_type(R, type).
 *
 * The standard types are as follows:
 *
 *   "basic" - implements the basic resource interface:
 *     type
 *     ping
 *     name
 *     open
 *     stat
 *     find
 *     get-access
 *     set-access
 *     get-key
 *
 *   "event" - is capable of (but not guaranteed to be) emitting events.
 *
 *   "file" - implements I/O handlers and the basic file interface:
 *     reset
 *     size
 *
 *   "dir" - implements the basic directory interface:
 *     list
 *     link
 *     unlink
 *
 *   "link" - implements the basic symbolic link interface:
 *     set-link
 *     get-link
 */

int rp_type(rp_t rp, const char *type);

#endif/*__RLIBC_RHOMBUS_H*/
