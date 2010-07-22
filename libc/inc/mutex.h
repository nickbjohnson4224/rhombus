/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#ifndef MUTEX_H
#define MUTEX_H

#include <stdbool.h>

bool mutex_lock(bool *mutex);
bool mutex_test(bool *mutex);
void mutex_spin(bool *mutex);
void mutex_wait(bool *mutex);
void mutex_free(bool *mutex);

#endif/*MUTEX_H*/
