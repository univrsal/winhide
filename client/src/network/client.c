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
    if (!network_state || !network_connected) {
        warn("Can't start client because connection to server failed.");
        return;
   } 
    
    window_list_t current, prev;
    prev.count = 0;
    prev.first = NULL;
    current.count = 0;
    current.first = NULL;

    while (network_loop) {
        bool send_windows = false;
        uint8_t window_count = 0;
        window_list_free(&current);
        network_buf->write_pos = 1; /* First byte will be the window count later */

        if (!window_list_build(&current)) {
            warn("Couldn't build Window list. Waiting %ims", config.refresh_rate);
            Sleep(config.refresh_rate);
            continue;
        }

        /* See if there's any windows we're interested in */
        target_t *t = config.first;
        while (t && window_count < 32) { /* Buffer only has space for 32 windows */
            window_t *w = window_list_find_first(&current, t);
            bool send_this_window = false;
            if (w) {
                window_t *w2 = window_list_find_first(&prev, t);

                /* Check if this window was here before and if its position changed */
                if (!w2 && (w->state & WINDOW_SHOWN)) {
                    /* Window is new and visible */
                    send_windows = true;
                    send_this_window = true;
                    debug("Found new window with title %s [x: %i, y: %i, w: %i, h: %i]", t->text,
                        w->x, w->y, w->width, w->height);
                } else if (w2 && compare_window(w, w2)) {
                    /* Window dimension changed */
                    if (w->state & WINDOW_SHOWN) {
                        send_windows = true;
                        send_this_window = true;
                        debug("Window with title %s moved [x: %i, y: %i, w: %i, h: %i]", t->text,
                            w->x, w->y, w->width, w->height);
                    } else if ((w2->state & WINDOW_SHOWN) && !(w->state & WINDOW_SHOWN)) {
                        /* Window was visible and isn't anymore,
                         * so we force the window list to be sent,
                         * even if it is empty. This makes sure that
                         * Windows that are being minimized are removed
                         * from the window list */
                        send_windows = true;
                    }
                }

                if (send_this_window) {
                    write_window(w);
                    window_count++;
                }
            }
            t = t->next;
        }

        if (send_windows) {
            network_buf->data[0] = window_count;
            if (netlib_tcp_send_buf(network_sock, network_buf) < 0) {
                warn("Couldn't send window data: %s", netlib_get_error());
            }
        }
        window_list_copy(&current, &prev);
        Sleep(config.refresh_rate);
    }
    SetConsoleCtrlHandler(CtrlHandler, FALSE);
}