/* 
 * Copyright 2009, 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details 
 */

#include <time.h>
#include <space.h>

pid_t curr_pid = 0;
task_t *curr_task = (void*) PROCESS_TABLE;
