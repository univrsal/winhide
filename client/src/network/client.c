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

#include "client.h"
#include "network.h"
#include "../util/util.h"
#include "../util/window_list.h"
#include "../util/config.h"

void write_window(window_t *w)
{
    netlib_write_uint16(network_buf, w->x);
    netlib_write_uint16(network_buf, w->y);
    netlib_write_uint16(network_buf, w->width);
    netlib_write_uint16(network_buf, w->height);
}

BOOL WINAPI CtrlHandler(DWORD dwCtrlType)
{
    network_loop = false;
    return TRUE;
}

void client_run(void)
{
    SetConsoleCtrlHandler(CtrlHandler, TRUE);
    if (!network_state || network_connected) {
        warn("Can't start client because connection to server failed.\n");
        return;
   } 
    
    window_list_t current, prev;
    prev.count = 0;
    prev.first = NULL;
    list.count = 0;
    list.first = NULL;

    while (!network_loop) {
        uint8_t window_count = 0;
        window_list_free(&current);
        network_buf->write_pos = 1; /* First byte will be the window count later */

        if (!window_list_build(&list)) {
            warn("Couldn't build Window list. Waiting %ims\n", config.refresh_rate);
            Sleep(config.refresh_rate);
            break;
        }

        /* See if there's any windows we're interested in */
        target_t *t = config.first;
        while (t) {
            window_t *w = window_list_find_first(&current, t);
            if (w && (w->state & WINDOW_SHOWN)) {
                window_t *w2 = window_list_find_first(&prev, t);
                bool send = false;
                /* Check if this window was here before and if it's position changed */
                if (!w2) {
                    /* Window is new */
                    send = true;
                    debug("Found new window with title %s [x: %i, y: %i, w: %i, h: %i]\n", t->text,
                        w->x, w->y, w->width, w->height);
                } else if (compare_window(w, w2)) {
                    /* Window dimension changed */
                    send = true;
                    debug("Window with title %s moved [x: %i, y: %i, w: %i, h: %i]\n", t->text,
                        w->x, w->y, w->width, w->height);
                }
                write_window(w);
                window_count++;
            }
            t = t->next;
        }

        if (window_count > 0) {
            network_buf->data[0] = window_count;
            if (netlib_tcp_send_buf(network_sock, network_buf) < 0) {
                warn("Couldn't send window data: %s\n", netlib_get_error());
            }
        }
        Sleep(config.refresh_rate);
    }
    SetConsoleCtrlHandler(CtrlHandler, FALSE);
}