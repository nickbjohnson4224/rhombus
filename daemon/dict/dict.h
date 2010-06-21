#ifndef DICT_H
#define DICT_H

struct dict_node {
	struct dict_node **nextv;

	struct dict_node *nexti;
	char   immediate;

	struct dict_node *link;
	char  *value;

	size_t refc;
};

int         dict_add(struct dict_node *dict, const char *key, const char *value);
int         dict_lnk(struct dict_node *dict, const char *key, const char *link);
int         dict_rem(struct dict_node *dict, const char *key);
const char *dict_get(struct dict_node *dict, const char *key);

#endif/*DICT_H*/
