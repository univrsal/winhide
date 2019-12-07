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

#ifndef CONFIG_H
#define CONFIG_H
#include <stdint.h>

typedef enum {
    search_exact = 1 << 0,
    search_fuzzy = 1 << 1,
    search_exe   = 1 << 2
} search_criteria;

typedef struct target_s {
    char text[256];
    search_criteria crit;
    struct target_s *next;
} target_t;

typedef struct config_s {
    uint16_t port;
    char addr[64];
    target_t *first;
    int target_count;
    int refresh_rate;
} config_t;

extern config_t config;

void config_init(void);

void config_close(void);
#endif