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
#include "config.h"
#define STR_LEN     256

typedef struct window_s {
	char title[STR_LEN];
    char executable[STR_LEN];
	int width, height, x, y;
	struct window_s *next;
    HWND handle;
    enum window_state {
        state_unknown,
        state_hidden,
        state_default,
        state_maximized,
        state_minimized,
    } state;
} window_t;

typedef struct window_list_s {
	window_t *first;
	int count;
} window_list_t;

int window_list_build(window_list_t *list);

void window_list_free(window_list_t *list);

window_t *window_list_find_first(window_list_t *list,
    const char* title, search_criteria crit);
#endif