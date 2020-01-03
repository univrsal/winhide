/*************************************************************************
 * This file is part of winhide
 * github.con/univrsal/winhide
 * Copyright 2019 univrsal <universailp@web.de>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/

#ifndef NETWORK_H
#define NETWORK_H
#include <Windows.h>
#include <netlib.h>
#include <stdbool.h>

extern volatile BOOL network_state;
extern volatile BOOL network_connected;
extern volatile BOOL network_loop;
extern ip_address network_addr;
extern tcp_socket network_sock;
extern netlib_socket_set network_socket_set;
extern netlib_byte_buf* network_buf;

bool network_start(void);

void network_close(void);
#endif