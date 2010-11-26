/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
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

#ifndef EXEC_H
#define EXEC_H

#include <stdint.h>

/* execution from exectuable image *****************************************/

int execiv(uint8_t *image, size_t size, char const **argv);
int execi (uint8_t *image, size_t size);

/* execution from exectuable file ******************************************/

int execv (const char *path, char const **argv);
int exec  (const char *path);

/* argument lists **********************************************************/

void   argv_pack  (int argc, const char **argv);
int    argc_unpack(void);
char **argv_unpack(void);

/* file descriptors ********************************************************/

void file_pack  (void);
void file_unpack(void);

#endif/*EXEC_H*/
