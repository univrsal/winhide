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

#include <stdio.h>
#include "../util/config.h"
#include "network.h"

volatile BOOL network_state = FALSE;
volatile BOOL network_connected = FALSE;
volatile BOOL network_loop = FALSE;
ip_address network_addr;
tcp_socket network_sock;
netlib_socket_set network_socket_set;
netlib_byte_buf *network_buf = NULL;

BOOL start_connection(void)
{
    if (netlib_resolve_host(&network_addr, config.addr, config.port) == -1) {
        printf("netlib_resolve_host failed: %s\n", netlib_get_error());
        printf("make sure obs-studio is running with a source listening\n");
        return FALSE;
    }

    network_socket_set = netlib_alloc_socket_set(1);
    if (!network_socket_set) {
        printf("netlib_alloc_socket_set failed: %s\n", netlib_get_error());
        return FALSE;
    }

    network_sock = netlib_tcp_open(&network_addr);
    if (!network_sock) {
        printf("netlib_tcp_open failed: %s\n", netlib_get_error());
        goto fail;
    }

    if (netlib_tcp_add_socket(network_socket_set, network_sock) == -1) {
        printf("netlib_tcp_add_socket failed: %s\n", netlib_get_error());
        goto fail;
    }

    return TRUE;
fail:
    netlib_free_socket_set(network_socket_set);
    return FALSE;
}

BOOL init(void)
{
    if (netlib_init() == -1) {
        printf("netlib_init failed: %s\n", netlib_get_error());
        return FALSE;
    }
    network_buf = netlib_alloc_byte_buf(255);
    if (!network_buf) {
        printf("netlib_alloc_byte_buf failed: %s\n", netlib_get_error());
        return FALSE;
    }
    network_state = TRUE;
    return TRUE;
}

bool network_start(void)
{
    bool result = true;
    if (!init()) {
        printf("Network init failed!\n");
        result = false;
    } else if (!start_connection()) {
        printf("Connection failed!\n");
        result = false;
    }
    return result;
}

void network_close(void)
{
    if (!network_state)
        return;
    network_state = FALSE;
    if (network_connected) {
        /* send DC msg? */
    }

    network_loop = FALSE;
    netlib_tcp_close(network_sock);
    netlib_free_byte_buf(network_buf);
    netlib_quit();
    network_buf = NULL;
}