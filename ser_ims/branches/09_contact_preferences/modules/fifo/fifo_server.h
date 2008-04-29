/*
 * $Id$
 *
 *
 * Copyright (C) 2001-2003 FhG Fokus
 * Copyright (C) 2005 iptelorg GmbH
 *
 * This file is part of ser, a free SIP server.
 *
 * ser is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version
 *
 * For a license to use the ser software under conditions
 * other than those described here, or to purchase support for this
 * software, please contact iptel.org by e-mail at the following addresses:
 *    info@iptel.org
 *
 * ser is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#ifndef _FIFO_SERVER_H
#define _FIFO_SERVER_H

#include <stdio.h>


#define CMD_SEPARATOR ':'

extern char* fifo;
extern char* fifo_dir;
extern char* fifo_user;
extern char* fifo_group;
extern int   fifo_mode;
extern int   fifo_reply_retries;
extern int   fifo_reply_wait;

/* Initialize FIFO server data structures */
int init_fifo_server(void);

/* Create the FIFO server process */
int start_fifo_server(void);

/* memory deallocation */
void destroy_fifo(void);

#endif
