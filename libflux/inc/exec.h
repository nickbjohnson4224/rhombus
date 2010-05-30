#ifndef EXEC_H
#define EXEC_H

#include <flux/arch.h>

int execiv(uint8_t *image, size_t size, char const **argv);
int execi (uint8_t *image, size_t size);

int execv (const char *path, char const **argv);
int exec  (const char *path);

#endif/*EXEC_H*/
