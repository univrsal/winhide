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

#ifndef WINDOW_LIST_H
#define WINDOW_LIST_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdbool.h>

#define STR_LEN         512 
#define WINDOW_SHOWN    0b1000
typedef struct target_s target_t;

typedef struct window_s {
	char title[STR_LEN];
    char executable[STR_LEN];
	int width, height, x, y;
	struct window_s *next;
    HWND handle;
    enum window_state {
        state_unknown       = 0b0000,
        state_hidden        = 0b0001,
        state_default       = 0b1010,
        state_maximized     = 0b1011,
        state_minimized     = 0b0100,
    } state;
} window_t;

typedef struct window_list_s {
	window_t *first;
	int count;
} window_list_t;

bool window_list_build(window_list_t *list);

void window_list_free(window_list_t *list);

void window_list_copy(const window_list_t *from, window_list_t *to);

void window_list_add_window(window_list_t *l, window_t *w);

void window_list_find_first(window_list_t *list, target_t *t);
#endif