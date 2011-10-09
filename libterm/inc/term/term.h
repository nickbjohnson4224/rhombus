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

#ifndef __LIBTERM_TERM_H
#define __LIBTERM_TERM_H

#include <robject.h>
#include <stdint.h>
#include <rdi/io.h>

/*****************************************************************************
 * class_term (extends rdi_class_file) - term
 */

extern struct robject *class_term;

struct robject *term_cons(uint32_t index, uint32_t access);
void            term_free(struct robject *r);

/* Calls: */

/*
 * get-color-fg - R
 *
 * Return: the current foreground color in #RRGGBB format.
 */

char *__term_get_color_fg(struct robject *self, rp_t src, int argc, char **argv);

/*
 * get-color-bg - R
 *
 * Return: the current background color in #RRGGBB format.
 */

char *__term_get_color_bg(struct robject *self, rp_t src, int argc, char **argv);

/*
 * set-color-fg <color> - W
 *
 * Change the current foreground color to <color> (in #RRGGBB format). The
 * color used by the terminal may be an approximation to this.
 */

char *__term_set_color_fg(struct robject *self, rp_t src, int argc, char **argv);

/*
 * set-color-bg <color> - W
 *
 * Change the current background color to <colo> (int #RRGGBB format). The
 * color used by the terminal may be an approximation to this.
 */

char *__term_set_color_fg(struct robject *self, rp_t src, int argc, char **argv);

/*
 * get-foreground - R
 *
 * Return: the pid of the foreground job for the terminal.
 */

char *__term_get_foreground(struct robject *self, rp_t src, int argc, char **argv);

/*
 * set-foreground <pid> - W
 *
 * Set the foreground job for the terminal. All signals (like Ctrl-C) will be
 * sent to the pid <pid> and its children.
 */

char *__term_set_foreground(struct robject *self, rp_t src, int argc, char **argv);

/*
 * get-columns - R
 *
 * Return: the current width of the terminal in characters.
 */

char *__term_get_columns(struct robject *self, rp_t src, int argc, char **argv);

/*
 * get-rows - R
 *
 * Return: the current height of the terminal in characters.
 */

char *__term_get_rows(struct robject *self, rp_t src, int argc, char **argv);

/* Fields:
 *
 * cols 
 *
 *   Number of columns in the terminal.
 *
 *   Type: uint32_t
 *
 * rows
 *
 *   Number of rows in the terminal.
 *
 *   Type: uint32_t
 *
 * color_fg
 *
 *   Pixel value of foreground color.
 *
 *   Type: uint32_t
 *
 * color_bg
 *
 *   Pixel value of background color.
 *
 *   Type: uint32_t
 *
 * foreground
 *
 *   PID of foreground process.
 *
 *   Type: uint32_t
 */

#endif/*__LIBTERM_TERM_H*/
