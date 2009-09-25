#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "inc/token.h"

int main() {
	struct token *t = new_token(TT_NIL, NULL, NULL), *t_end = t;
	int i;
	char c;

	FILE *source = fopen("source.sl", "r");
	if (!source) return 1;

	tokenizer_init();
	while ((c = getc(source)) != EOF) 
		t_end = tokenize_char(c, t_end);
	t_end = tokenizer_flush(t_end);

	while (t) {
		if (t->type != TT_NIL) printf("%s ", t->lexeme);
		t = t->next_token;
	}
	printf("\n");

	return 0;
}
