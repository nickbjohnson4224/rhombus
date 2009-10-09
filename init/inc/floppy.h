#ifndef INIT_FLOPPY_H
#define INIT_FLOPPY_H

#include <khaos/driver.h>

int  init_floppy(uint16_t selector);
void floppy_handler(void);

void    floppy_write_cmd(uint8_t cmd);
uint8_t floppy_read_data(void);

extern struct driver_interface floppy;

#endif
