#ifndef TOKEN_H
#define TOKEN_H

typedef struct token {
	unsigned char type;
	char *symbol;
} token_t;

#define T_SET 0x0

#define T_OPAREN 0x1
#define T_CPAREN 0x2
#define T_OBRACE 0x3
#define T_CBRACE 0x4
#define T_SEMICOLON 0x5

#define T_ADD 0x8
#define T_SUB 0x9
#define T_MUL 0xA
#define T_DIV 0xB
#define T_MOD 0xC
#define T_INC 0xD
#define T_DEC 0xE

#define T_LIOR 0x10
#define T_LAND 0x11
#define T_LNOT 0x12
#define T_LXOR 0x13
#define T_LLSH 0x14
#define T_LRSH 0x15

#define T_BEQU 0x18
#define T_BLSS 0x19
#define T_BGTR 0x1A
#define T_BLSE 0x1B
#define T_BGTE 0x1C
#define T_BIOR 0x1D
#define T_BAND 0x1E
#define T_BNOT 0x1F

#define T_FOR 0x20
#define T_WHILE 0x21
#define T_DO 0x22
#define T_IF 0x23
#define T_ELSE 0x24
#define T_TIF 0x25
#define T_TELSE 0x26
#define T_GOTO 0x27

#define T_STRCT 0x30
#define T_UNION 0x31
#define T_TYPED 0x32
#define T_RETURN 0x33

#define T_TINT 0x38
#define T_TSHORT 0x39
#define T_TCHAR 0x3A
#define T_TVOID 0x3B
#define T_TFLOAT 0x3C
#define T_TDOUBLE 0x3D
#define T_TLONG 0x3E

#define T_TUNSIGN 0x40
#define T_TSIGN 0x41
#define T_TREGIST 0x42
#define T_TINLINE 0x43
#define T_TEXTERN 0x44
#define T_TSTATIC 0x45
#define T_TCONST 0x46

#define T_EOL 0x80
#define T_TBD 0x81

#define T_CLITERAL 0xC0
#define T_SLITERAL 0xC1
#define T_NLITERAL 0xC2
#define T_SYMBOL 0xC3

int tokenize_line(char *line, token_t *token);

#endif
