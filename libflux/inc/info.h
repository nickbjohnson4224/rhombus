#ifndef FLUX_INFO_H
#define FLUX_INFO_H

extern struct info_node {
	size_t next[96];
	char  value[128];
} *infospace;

extern uint32_t  m_info;
extern uintptr_t infobrk;

const char *getinfo(const char *name);
void        setinfo(const char *name, const char *value);
void        delinfo(const char *name);
char       *lsinfo (const char *prefix);
char       *lsninfo(const char *prefix, char *buffer, size_t n);

size_t	info_node_alloc(void);
void    info_node_free (size_t n);

#endif/*FLUX_INFO_H*/
