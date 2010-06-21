#ifndef FLUX_DICT_H
#define FLUX_DICT_H

char *dfind(const char *key, char *value);
char *dlink(const char *key, const char *link);
char *dremv(const char *key);
char *dinst(const char *key, char *value);
char *dcmd(const char *cmd, const char *key, char *value);

struct dict_query {
	char cmd[12];
	char key[2060];
	char val[1024];
};

#endif/*FLUX_DICT_H*/
