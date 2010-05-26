#ifndef EXEC_H
#define EXEC_H

#include <flux/arch.h>

int execiv(uint8_t *image, size_t size, char **argv);
int execi (uint8_t *image, size_t size);

#endif/*EXEC_H*/
