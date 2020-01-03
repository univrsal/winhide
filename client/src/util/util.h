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

#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <stdio.h>
/* Logging */
#define info(format, ...) printf("[info]  " format "\n", ##__VA_ARGS__)
#ifdef _DEBUG
#define debug(format, ...) printf("[debug] " format "\n", ##__VA_ARGS__)
#else
#define debug(format, ...)
#endif
#define warn(format, ...) printf("[warn]  " format "\n", ##__VA_ARGS__)

#define strbool(b) (b ? "true" : "false")

#define compare_window(a, b) (a->x != b->x || a->y != b->y || a->width != b->width || a->height != b->height)
#endif