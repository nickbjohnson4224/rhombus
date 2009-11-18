#ifndef CTYPE_H
#define CTYPE_H

#include <stdint.h>

extern uint16_t _ctype_tbl[256];

#define isalnum(c) (_ctype_tbl[c] & 0x001)
#define isalpha(c) (_ctype_tbl[c] & 0x002)
#define isblank(c) (_ctype_tbl[c] & 0x004)
#define iscntrl(c) (_ctype_tbl[c] & 0x008)
#define isdigit(c) (_ctype_tbl[c] & 0x010)
#define isgraph(c) (_ctype_tbl[c] & 0x020)
#define islower(c) (_ctype_tbl[c] & 0x040)
#define isprint(c) (_ctype_tbl[c] & 0x080)
#define ispunct(c) (_ctype_tbl[c] & 0x100)
#define isspace(c) (_ctype_tbl[c] & 0x200)
#define isupper(c) (_ctype_tbl[c] & 0x400)
#define isxdigit(c)(_ctype_tbl[c] & 0x800)

#endif/*CTYPE_H*/
