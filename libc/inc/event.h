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

#ifndef __RLIBC_EVENT_H
#define __RLIBC_EVENT_H

#include <robject.h>
#include <stdint.h>

/**************************************************************************** 
 * Rhombus Object Event System (event)
 *
 * The event protocol is an asyncronous, broadcasting parallel of the rcall 
 * protocol. Only a single ASCII string is sent as event data, and events are 
 * sent from robjects to processes. Each robject maintains a list of "event 
 * subscribers", to which messages are sent if an event is to be sent from 
 * that robject. Think of it as an RSS feed.
 *
 * Instead of a method name like in rcall, event uses the first token of the
 * argument string as an event type, which is used to route it. Event types
 * should be used to group similar events together (like keypress events, or
 * mouse movement events, or window events.)
 */

// manage event sources
int event_subscribe  (rp_t event_source);
int event_unsubscribe(rp_t event_source);

// event hook format
typedef void (*event_t)(rp_t src, int argc, char **argv);

// set the event hook for the given event type
int event_hook(const char *type, event_t hook);

// send an event
int event(rp_t rp, const char *value);

#endif/*__RLIBC_EVENT_H*/
