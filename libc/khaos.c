#include <stdint.h>
#include <stdlib.h>
#include <khaos.h>
#include <config.h> 

void block() {
	_ctrl(CTRL_SCHED, CTRL_SCHED);
}

void unblock() {
	_ctrl(CTRL_NONE, CTRL_SCHED);
}

/*size_t read(file_t *fd, char *buffer, size_t count) {
	request_t *page = malloc(PAGESZ);
}

size_t write(file_t *fd, char *buffer, size_t count); */
