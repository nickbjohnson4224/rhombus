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

#include <stdlib.h>
#include <stdio.h>
#include <natio.h>
#include <errno.h>

int main(int argc, char **argv) {
	FILE *file;

	if (argc < 2) {
		fprintf(stderr, "cons: missing file operand\n");
		return 1;
	}
	else {
		
		if (argv[1][0] == '-' && argc > 2) {
			file = vfs_get_file(NULL, argv[2]);

			if (file) {
				printf("cons: object %s exists\n", argv[2]);
				fclose(file);
				return 1;
			}

			switch (argv[1][1]) {
			case 'f': /* construct file */
				file = vfs_new_file(NULL, argv[2]);
				break;
			case 'd': /* construct directory */
				file = vfs_new_dir(NULL, argv[2]);
				break;
			case 'l': /* construct link */
				if (argc < 4) {
					printf("cons: no link operand\n");
					return 1;
				}

				if (argc < 5) {
					file = vfs_new_link(NULL, argv[2], argv[3], NULL);
				}
				else {
					file = vfs_new_link(NULL, argv[2], argv[3], 
						__fcons(atoi(argv[4]), atoi(argv[5]), NULL));
				}

				break;
			default:
				file = NULL;
			}
		}
		else {
			file = vfs_get_file(NULL, argv[1]);

			if (file) {
				printf("cons: object %s exists\n", argv[1]);
				fclose(file);
				return 1;
			}

			file = vfs_new_file(NULL, argv[1]);
		}

		if (!file) {
			printf("cons: cannot construct object: \n");
			perror(NULL);
		}
		else {
			fclose(file);
		}
	}
	
	return 0;
}
