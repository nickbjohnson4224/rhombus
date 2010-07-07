/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/ipc.h>

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

uint32_t m_msg_queue[256];
