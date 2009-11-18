#ifndef STDINT_H
#define STDINT_H

#include <stddef.h>

/* Precise size integers */
typedef unsigned char	uint8_t;
typedef unsigned short	uint16_t;
typedef unsigned int	uint32_t;
typedef signed char		int8_t;
typedef signed short	int16_t;
typedef signed int		int32_t;

/* Minimum size integers */
typedef uint32_t		uint_least8_t;
typedef uint32_t		uint_least16_t;
typedef uint32_t		uint_least32_t;
typedef int32_t			int_least8_t;
typedef int32_t			int_least16_t;
typedef int32_t			int_least32_t;

/* Fast integers */
typedef uint8_t			uint_fast8_t;
typedef uint16_t		uint_fast16_t;
typedef uint32_t		uint_fast32_t;
typedef int8_t			int_fast8_t;
typedef int16_t			int_fast16_t;
typedef int32_t			int_fast32_t;

/* Pointer equivalents */
typedef uint32_t		uintptr_t;
typedef int32_t			intptr_t;

/* Maximum size integers */
typedef uint32_t		uintmax_t;
typedef int32_t			intmax_t;

#endif
