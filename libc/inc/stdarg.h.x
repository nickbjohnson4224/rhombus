#ifndef STDARG_H
#define STDARG_H

#include <stdint.h>

typedef uint8_t* va_list;

#define va_start(ap, arg) (ap = (void*) ((uintptr_t) arg + sizeof(arg)))
#define va_arg(ap, type)  (((uintptr_t) ap += sizeof(type)) - sizeof(type))
#define va_end(ap) NULL

#endif/*STDARG_H*/
