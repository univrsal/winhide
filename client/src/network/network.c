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

#include <Windows.h>
#include <stdio.h>
#include <netlib.h>
#include "../util/config.h"
#include "network.h"

static volatile BOOL state = FALSE;
static volatile BOOL connected = FALSE;
static volatile BOOL loop = FALSE;
static ip_address addr;
static tcp_socket sock;
static netlib_socket_set set;
static netlib_byte_buf *buf = NULL;
static HANDLE network_thread;

DWORD WINAPI thread_method(LPVOID arg)
{

}

BOOL start_thread()
{
    network_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_method,
        NULL, 0, NULL);
    return network_thread;
}

BOOL start_connection(void)
{
    if (netlib_resolve_host(&addr, config.addr, config.port) == -1) {
        printf("netlib_resolve_host failed: %s\n", netlib_get_error());
        printf("make sure obs-studio is running with a source listening\n");
        return FALSE;
    }

    set = netlib_alloc_socket_set(1);
    if (!set) {
        printf("netlib_alloc_socket_set failed: %s\n", netlib_get_error());
        return FALSE;
    }

    sock = netlib_tcp_open(&addr);
    if (!sock) {
        printf("netlib_tcp_open failed: %s\n", netlib_get_error());
        goto fail;
    }

    if (netlib_tcp_add_socket(set, sock) == -1) {
        printf("netlib_tcp_add_socket failed: %s\n", netlib_get_error());
        goto fail;
    }



    return TRUE;
fail:
    netlib_free_socket_set(set);
    return FALSE;
}

BOOL init(void)
{
    if (netlib_init() == -1) {
        printf("netlib_init failed: %s\n", netlib_get_error());
        return FALSE;
    }
    buf = netlib_alloc_byte_buf(17);
    if (!buf) {
        printf("netlib_alloc_byte_buf failed: %s\n", netlib_get_error());
        return FALSE;
    }
    state = TRUE;
    return TRUE;
}

void network_start(void)
{
    if (!init()) {
        printf("Network init failed!\n");
    } else if (!start_connection()) {
        printf("Connection failed!\n");
    }
}

void network_close(void)
{
    if (!state)
        return;
    state = FALSE;
    if (connected) {
        /* send DC msg? */
    }

    loop = FALSE;
    netlib_tcp_close(sock);
    netlib_free_byte_buf(buf);
    netlib_quit();
    buf = NULL;
}