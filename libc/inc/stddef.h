#ifndef STDDEF_H
#define STDDEF_H

#include <stdint.h>

typedef intptr_t  ptrdiff_t;
typedef uint32_t  wchar_t;

#define PTRDIFF_MIN	INTPTR_MIN
#define PTRDIFF_MAX	INTPTR_MAX

#define WCHAR_MIN	0
#define WCHAR_MAX	UINT32_MAX

#endif/*STDDEF_H*/
