#ifndef INIT_CONSOLE_H
#define INIT_CONSOLE_H

void sclear(void);
void cwrite(char c);
void swrite(const char *s);
void curse(int y, int x);
void gets(char *buf);

void update_progress(const char *message);
void print_bootsplash(void);

void kbhandle(void);

#endif
