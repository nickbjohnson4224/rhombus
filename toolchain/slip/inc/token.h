#ifndef TOKEN_H
#define TOKEN_H


#define TT_NIL		0	// Invalid token
#define TT_LEVEL1	1	// Output from level 1 tokenizer
#define TT_SYMBOL	2	// Uncategorized symbol
#define TT_OPAREN	3	// Opening parenthesis
#define TT_CPAREN	4	// Closing parenthesis
#define TT_STRING	5	// String literal
#define TT_APOST	6	// Apostraphe
#define TT_COLON	7	// Cons colon
#define TT_INDEX	8	// Pointer index

struct token {
	uint8_t type;
	char *lexeme;
	struct token *sub_token;
	struct token *next_token;
};

/* Pass the next character to the tokenizer */
struct token *tokenize_char(char c, struct token *t);

/* Make sure everything has been tokenized */
struct token *tokenizer_flush(struct token *t);

/* Prepare the tokenizer for tokenizing */
void tokenizer_init();

/* Create new token and add to list */
struct token *new_token(uint8_t type, char *lexeme, struct token *t);

#define MAX_TOKEN_SIZE 16

#endif
