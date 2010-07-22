/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdlib.h>
#include <ipc.h>

/****************************************************************************
 * msg_queue
 *
 * Message queues for each port.
 */

struct message msg_queue[256];

/****************************************************************************
 * m_msg_queue
 *
 * Mutexen for each port's message queue.
 */

bool m_msg_queue[256];
