#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/token.h"

/* This file consititutes a very simple tokenizer, one with only one character of state. It does not use a regular expression or finite state machine to tokenize, because tokens are either parens, apostraphes, or strings of any other character except whitespace. The following table sorts characters into different types with different rules. Whitespace signifies the end of any token. Switching types signifies the end of any token. All characters between and including a pair of quotes are one token. All characters between a comment and a newline are ignored. A second stage tokenizer creates sub-tokens, like array indicies and colon-conses. */

/* 0 - whitespace, 1 - symbol, 2 - paren/apostraphe, 3 - comment, 4 - quote, 5 - newline, 6 - escape */
const static uint8_t token_type[128] = {
/*	NUL	SOH	STX	ETX	EOT	ENQ	ACK	BEL	BS	TAB	LF	VT	FF	CR	SO	SI 	*/
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	5,	0,	0,

/*	DLE	DC1	DC2	DC3	DC4	NAK	SYN	ETB	CAN	EM	SUB	ESC	FS	GS	RS	US 	*/
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,

/*	SPC	!	"	#	$	%	&	'	(	)	*	+	,	-	.	/	*/
	0,	1,	4,	3,	1,	1,	1,	2,	2,	2,	1,	1,	1,	1,	1,	1,

/*	0	1	2	3	4	5	6	7	8	9	:	;	<	=	>	?	*/
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	3,	1,	1,	1,	1,

/*	@	A	B	C	D	E	F	G	H	I	J	K	L	M	N	O	*/
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,

/*	P	Q	R	S	T	U	V	W	X	Y	Z	[	\	]	^	_	*/
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	6,	1,	1,	1,

/* `	a	b	c	d	e	f	g	h	i	j	k	l	m	n	o	*/
	4,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,

/*	p	q	r	s	t	u	v	w	x	y	z	{	|	}	~	DEL	*/
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	0
};

/* Tokenizer state */
static uint8_t prev_type = 0;
static char *buffer = NULL;
static uint8_t buffer_end = 0;
static bool in_quote = false;
static bool in_comment = false;

/* Prepare the tokenizer for tokenizing */
void tokenizer_init() {
	buffer = malloc(sizeof(char) * (MAX_TOKEN_SIZE + 1));
	buffer_end = 0;
}

/* Pass the next character to the tokenizer */
struct token *tokenize_char(char c, struct token *t) {
	char *finished_lexeme;
	uint8_t type;
	struct token *temp_t;
	
	type = token_type[(uint8_t) c];

	/* Check whether in or out of a comment */
	if (in_comment) {
		if (type == 5) in_comment = false;
		return t;
	}
	if (type == 3 && in_quote == false) {
		in_comment = true;
		return t;
	}

	/* Check whether in or out of a string */
	if (type == 4 && prev_type != 6) {
		in_quote = !in_quote;
		return t;
	}

	/* If a new token needs to be started... */
	if ((type != prev_type || type == 2) && !in_quote && prev_type != 0) {

		/* Save and terminate finished lexeme */
		finished_lexeme = buffer;
		if (finished_lexeme) finished_lexeme[buffer_end + 1] = '\0';

		/* Allocate and initialize buffer for next lexeme */
		buffer = malloc(sizeof(char) * (MAX_TOKEN_SIZE + 1));
		buffer_end = 0;

		/* Create new token from lexeme */
		t->next_token = new_token(TT_LEVEL1, finished_lexeme, NULL);
		t = t->next_token;
	}

	/* Add the character to the lexeme buffer if not whitespace */
	if (type != 0 || in_quote) buffer[buffer_end++] = c;

	/* Set prev_type for next call */
	prev_type = type;

	return t;
}

/* Make sure everything has been tokenized */	
struct token *tokenizer_flush(struct token *t) {
	char *finished_lexeme;

	finished_lexeme = buffer;
	finished_lexeme[buffer_end + 1] = '\0';

	prev_type = 0;
	buffer = NULL;
	buffer_end = 0;

	t->next_token = new_token(TT_LEVEL1, finished_lexeme, NULL);
	return t->next_token;
}

/* Create new token and add to list */
struct token *new_token(uint8_t type, char *lexeme, struct token *t) {
	struct token *temp;

	temp = malloc(sizeof(struct token));
	temp->type = type;
	temp->lexeme = lexeme;
	temp->sub_token = NULL;
	temp->next_token = t;

	return temp;
}
