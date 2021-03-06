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
#include "config.h"
#include "util.h"
#include <handleapi.h>
#include <stdio.h>
#include <string.h>

inline BOOL grab_window_title(window_t* w)
{
    return GetWindowText(w->handle, w->title, STR_LEN) > 0;
}

inline BOOL grab_window_dim(window_t* w)
{
    RECT dim = { 0, 0, 0, 0 };
    w->dim.x = 0;
    w->dim.y = 0;
    w->dim.w = 0;
    w->dim.h = 0;

    if (GetWindowRect(w->handle, &dim)) {
        w->dim.w = max(dim.right - dim.left, 0);
        w->dim.h = max(dim.bottom - dim.top, 0);
        w->dim.x = dim.left;
        w->dim.y = dim.top;
        return TRUE;
    }
    return FALSE;
}

inline BOOL grab_window_state(window_t* w)
{
    WINDOWPLACEMENT placement;
    placement.length = sizeof(WINDOWPLACEMENT);
    if (GetWindowPlacement(w->handle, &placement)) {
        switch (placement.showCmd) {
        case SW_HIDE:
            w->state = state_hidden;
            break;
        case SW_SHOWNORMAL:
        case SW_SHOW:
        case SW_SHOWNOACTIVATE:
        case SW_SHOWNA:
            w->state = state_default;
            break;
        case SW_MAXIMIZE:
        case SW_MAX:
            w->state = state_maximized;
            break;
        case SW_MINIMIZE:
        case SW_SHOWMINIMIZED:
            w->state = state_minimized;
        }
        return TRUE;
    }
    return FALSE;
}

inline BOOL grab_window_exe(window_t* w)
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

inline BOOL rect_is_covered(const struct rect* compare, const struct rect* with)
{
    return with->x <= compare->x && with->y <= compare->y &&
        with->w >= compare->w && with->h >= compare->h;
}

BOOL window_is_covered(const window_t* w, HWND next)
{
    window_t tmp;
    
    while (next) {
        tmp.handle = next;
        if (grab_window_dim(&tmp)) {
            if (rect_is_covered(&w->dim, &tmp.dim))
                return TRUE;
        }
        GetNextWindow(next, GW_HWNDPREV);
    }
    return FALSE;
}

BOOL CALLBACK enum_callback(HWND handle, LPARAM data)
{
    window_list_t* list = data;
    window_t* new_window = malloc(sizeof(window_t));
    new_window->title[0] = '\0';
    new_window->executable[0] = '\0';
    new_window->next = NULL;
    new_window->state = state_unknown;
    new_window->handle = handle;

    if (grab_window_title(new_window) && grab_window_dim(new_window) && grab_window_state(new_window) && grab_window_exe(new_window)) {
        HWND next = new_window->handle;
        GetNextWindow(next, GW_HWNDPREV);
        new_window->covered = window_is_covered(new_window, next);
        window_list_add_window(list, new_window);
    } else {
        /* Doesn't have a title or size */
        free(new_window);
    }
    return TRUE;
}

void window_list_add_window(window_list_t* l, window_t* w)
{
    if (!l || !w)
        return;
    w->next = l->first;
    l->first = w;
    l->count++;
}

bool window_list_build(window_list_t* list)
{
    list->count = 0;
    list->first = NULL;

    if (list && EnumWindows(enum_callback, list)) {
        return true;
    } else {
        return false;
    }
}

void window_list_free(window_list_t* list)
{
    if (list) {
        window_t *curr = list->first,
                 *next = NULL;
        while (curr) {
            next = curr->next;
            free(curr);
            curr = next;
            list->count--;
        }
        list->count = 0;
        list->first = NULL;
    }
}

window_t* window_list_find_first(window_list_t* list, target_t* t)
{
    if (!t || !t->text || !list)
        return NULL;
    window_t* curr = list->first;

    while (curr) {
        BOOL title = FALSE, exe = FALSE;
        if (t->crit & search_exact)
            title = strcmp(curr->title, t->text) == 0;
        else
            title = strstr(curr->title, t->text);

        if (t->crit & search_exe) /* Exe always fuzzy */
            exe = strstr(curr->title, t->text);
        else
            exe = TRUE;

        if (exe && title) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

void window_copy(const window_t* w, window_t* t)
{
    t->state = w->state;
    t->handle = w->handle;
    t->next = NULL;
    t->dim.w = w->dim.w;
    t->dim.h = w->dim.h;
    t->dim.x = w->dim.x;
    t->dim.y = w->dim.y;
    strncpy(t->title, w->title, STR_LEN);
    strncpy(t->executable, w->executable, STR_LEN);
}

void window_list_copy(const window_list_t* from, window_list_t* to)
{
    if (!from || !to)
        return;
    window_list_free(to);

    window_t* cur = from->first;
    while (cur) {
        window_t* copy = malloc(sizeof(window_t));
        window_copy(cur, copy);
        window_list_add_window(to, copy);
        cur = cur->next;
    }
}
