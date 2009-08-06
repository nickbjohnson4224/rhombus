#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "token.h"

static const char *token_list[] = {
"=",		"(",		")",		"{",		"}",		";",		"", 		"",
"+",		"-",		"*",		"/",		"%",		"++",		"--", 		"",
"|",		"&",		"~",		"^",		"<<",		">>",		"", 		"",
"==",		"<",		">",		"<=",		">=",		"||",		"&&",		"!",
"for",		"while",	"do",		"if",		"else",		"?",		":",		"goto",
"struct",	"union",	"typedef",	"return",	"",			"",			"",			"",
"int",		"short",	"char",		"void",		"float",	"double",	"long",		"",
"unsigned",	"signed",	"register",	"inline",	"extern",	"static",	"const",	""};

// 0-skippable 1-alphanumeric 2-symbolic 3-separator 4-illegal 5-string
static const char token_order[] = {
4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 0, 4, 4,
4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
0, 2, 5, 4, 2, 2, 5, 1, 3, 3, 2, 2, 3, 2, 0, 2,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2,
4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 3, 2, 1,
4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 2, 3, 2, 4};

int main() {
	token_t *tokens = malloc(sizeof(token_t)*4096);
	char *buffer = malloc(sizeof(char)*4096);
	while(1) {
		if (fgets(buffer, 4096, stdin) == NULL) return 0;
		if (tokenize_line(buffer, tokens)) continue;
		int i;
		for (i = 0; tokens[i].type != T_EOL; i++) {
			if (tokens[i].type < 0xC0)
				printf("%s ", token_list[tokens[i].type]);
			else
				printf("%s ", tokens[i].symbol);
		}
//		if (i) printf("\n");
	}
	return 0;
}

static int tokenize(char *buffer, token_t *token) {
	int i;

	for (i = 0; i < 64; i++) if (!strcmp(token_list[i], buffer)) break;
	if (i >= 64) {
		if (buffer[0] == '\'') token->type = T_CLITERAL;
		else if (buffer[0] == '\"') token->type = T_SLITERAL;
		else if (buffer[0] >= '0' && buffer[0] <= '9') token->type = T_NLITERAL;
		else token->type = T_SYMBOL;
		token->symbol = malloc(strlen(buffer)+sizeof(char));
		strcpy(token->symbol, buffer);
	}
	else token->type = i;
	return 0;
}

int tokenize_line(char *line, token_t *tokens) {
	char *focus, buffer[64], order = 0, instring = 0;
	int bufftop = 0;
	token_t *token_top = &tokens[0];

	for (focus = line; *focus != '\0'; focus++) {
		if (instring) order = token_order[*focus];
		if (token_order[*focus] == 5) {
			instring = !instring;
		}
		if (token_order[*focus] != order || (token_order[*focus] == 3 && !instring)) {
			buffer[bufftop] = '\0';
			if (order != 0) tokenize(buffer, token_top++);
			if (token_order[*focus] == 4) return -1;
			bufftop = 0;
			order = token_order[*focus];
		}
		buffer[bufftop++] = *focus;
	}

	buffer[bufftop] = '\0';
	if (order != 0) tokenize(buffer, token_top++);

	token_top->type = T_EOL;
	return 0;
}
