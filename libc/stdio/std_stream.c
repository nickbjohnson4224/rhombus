/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdio.h>

/****************************************************************************
 * stdin
 *
 * Standard input stream. Uses file descriptor 0.
 */

FILE *stdin  = NULL;

/****************************************************************************
 * stdout
 *
 * Standard output stream. Uses file descriptor 1.
 */

FILE *stdout = NULL;

/****************************************************************************
 * stderr
 *
 * Standard error stream. Uses file descriptor 2.
 */

FILE *stderr = NULL;

/****************************************************************************
 * stdvfs
 *
 * Virtual filesystem stream. Uses file descriptor 3.
 */

FILE *stdvfs = NULL;

/****************************************************************************
 * stddev
 *
 * Device manager stream. Uses file descriptor 4.
 */

FILE *stddev = NULL;

/****************************************************************************
 * stdpmd
 *
 * Process metadata stream. Uses file descriptor 5.
 */

FILE *stdpmd = NULL;

/****************************************************************************
 * extin
 *
 * Extended input stream. Uses file descriptor 6.
 */

FILE *extin  = NULL;

/****************************************************************************
 * extout
 *
 * Extended output stream. Uses file descriptor 7.
 */

FILE *extout = NULL;
