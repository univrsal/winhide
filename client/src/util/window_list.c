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

#include "window_list.h"
#include <stdio.h>
#include <handleapi.h>

inline BOOL grab_window_title(window_t *w)
{
    return GetWindowText(w->handle, w->title, STR_LEN) > 0;
}

inline BOOL grab_window_dim(window_t *w)
{
    RECT dim = { 0, 0, 0, 0 };
    w->x = 0; w->y = 0; w->width = 0; w->height = 0;

    if (GetWindowRect(w->handle, &dim)) {
        w->width = max(dim.right - dim.left, 0);
        w->height = max(dim.bottom - dim.top, 0);
        w->x = dim.left;
        w->y = dim.top;
        return TRUE;
    }
    return FALSE;
}

inline BOOL grab_window_state(window_t *w)
{
    WINDOWPLACEMENT placement;
    placement.length = sizeof(WINDOWPLACEMENT);
    if (GetWindowPlacement(w->handle, &placement)) {
        switch (placement.showCmd) {
        case SW_HIDE:
            w->state = state_hidden;
            break;
        case SW_SHOWNORMAL:
            w->state = state_default;
            break;
        case SW_MAXIMIZE:
            w->state = state_maximized;
            break;
        case SW_MINIMIZE:
            w->state = state_minimized;
        }
        return TRUE;
    }
    return FALSE;
}

inline BOOL grab_window_exe(window_t *w)
{
    BOOL result = FALSE;
    DWORD proc_id = 0, length = STR_LEN;
    HANDLE h = NULL;
    GetWindowThreadProcessId(w->handle, &proc_id);
    h = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, proc_id);
    if (h)
        result = QueryFullProcessImageName(h, NULL, w->executable, &length);
    CloseHandle(h);
    return result;
}

BOOL CALLBACK enum_callback(HWND handle, LPARAM data)
{
	window_list_t *list = data;
	window_t *new_window = malloc(sizeof(window_t));
    new_window->title[0] = '\0';
    new_window->executable[0] = '\0';
    new_window->next = NULL;
    new_window->state = state_unknown;
    new_window->handle = handle;

    BOOL a = grab_window_title(new_window);
    BOOL b = grab_window_dim(new_window);
    BOOL c = grab_window_state(new_window);
    BOOL d = grab_window_exe(new_window);

	if (grab_window_title(new_window) &&
        grab_window_dim(new_window) &&
        grab_window_state(new_window) &&
        grab_window_exe(new_window))
    {    
        if (list->first) {
            window_t *old = list->first;
            list->first = new_window;
            new_window->next = old;
        } else {
            list->first = new_window;
        }
        list->count++;
    } else {
        /* Doesn't have a title or size */
        free(new_window);
    }
    return TRUE;
}

int window_list_build(window_list_t *list)
{
    list->count = 0;
    list->first = NULL;

	if (list && EnumWindows(enum_callback, list)) {
		return 1;
	} else {
		return -1;
	}
}

void window_list_free(window_list_t *list)
{
    if (list) {
        window_t *curr = list->first,
                 *next = NULL;
        while (curr) {
            next = curr->next;
            free(curr);
            curr = next;
        }
    }
}

window_t *window_list_find_first(window_list_t *list, 
    const char* title, search_criteria crit)
{
    if (!title || !list)
        return NULL;
    window_t *curr = list->first;

    while (curr) {
        BOOL title = FALSE, exe = FALSE;
        if (crit & search_exact)
            title = strcmp(curr->title, title) == 0;
        else
            title = strstr(curr->title, title);

        if (crit & search_exe) /* Exe always fuzzy */
            exe = strstr(curr->title, title);
        else
            exe = TRUE;

        if (exe && title) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}