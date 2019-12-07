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

#include "config.h"
#include "util.h"
#include <jansson.h>
#include <string.h>
#define CFG_PATH    "./winhide.json"

#define CFG_PORT            "port"
#define CFG_TARGETS         "targets"
#define CFG_TARGET_TEXT     "text"
#define CFG_SEARCH_EXACT    "exact"
#define CFG_SEARCH_EXE      "search_exe"
#define CFG_IP_ADDR         "server_ip"
#define CFG_REFRESH_RATE    "refresh_rate"

config_t config;

void target_add(const char *text, search_criteria flags)
{
    target_t *n = malloc(sizeof(target_t));
    n->crit = flags;
    n->next = config.first;
    strncpy(n->text, text, 256);
    config.first = n;
    config.target_count++;
    info("Added target \"%s\" (Exact: %s, Exe: %s)\n", text,
        strbool(flags & search_exact), strbool(flags & search_exe));
}

void init_defaults(void)
{
    /* Defaults */
    config.target_count = 0;
    config.first = NULL;
    config.port = 16899;
    config.refresh_rate = 250;
    strcpy(config.addr, "127.0.0.1");
    target_add("TeamViewer", search_exact | search_exe);
}

int config_load(json_t *j)
{
    json_t *a = NULL, *val = NULL;
    size_t i;
    json_error_t e;
    bool result = true;

    if (json_unpack_ex(j, &e, 0, "{si,ss,so}", CFG_PORT,
        &config.port, CFG_IP_ADDR, &config.addr, CFG_TARGETS, &a) == 0) {
        /* Unpack targets */
        int crit = 0;
        int exe = false, exact = false;
        char buf[256];
        char *tmp;

        json_array_foreach(a, i, val) {
            if (json_unpack_ex(val, &e, 0, "{ss,sb,sb}", 
                CFG_TARGET_TEXT, &tmp, CFG_SEARCH_EXACT, &exact,
                CFG_SEARCH_EXE, &exe) == 0) {
                crit |= exact ? search_exact : search_fuzzy;
                crit |= exe ? search_exe : 0;
                strcpy_s(buf, 256, tmp);
                target_add(buf, crit);
            } else {
                printf("Error unpacking config json: %s (line: %i, col: %i)\n",
                    e.text, e.line, e.column);
            }
        }
    } else {
        printf("Error unpacking config json: %s (line: %i, col: %i)\n",
            e.text, e.line, e.column);
        result = false;
    }

    return result;
}

int config_create()
{
    /* Encode targets */
    json_error_t e;
    json_t *a = json_array();
    target_t *curr = config.first;
    bool result = true;
    while (curr) {
        json_t *t_json = json_pack_ex(&e, 0, "{ss,sb,sb}",
            CFG_TARGET_TEXT, curr->text,
            CFG_SEARCH_EXACT, curr->crit & search_exact,
            CFG_SEARCH_EXE, curr->crit & search_exe);
        if (t_json) {
            json_array_append_new(a, t_json);
        } else {
            printf("Error while packing target in json: %s\n",
                e.text);
            result = false;
        }
        curr = curr->next;
    }
 
    json_t *cfg = json_pack_ex(&e, 0, "{si,ss,so}",
        CFG_PORT, config.port, CFG_IP_ADDR, config.addr,
        CFG_TARGETS, a);
    if (cfg && result) {
        json_dump_file(cfg, CFG_PATH, JSON_INDENT(4));
    } else {
        printf("Error while packing config in json: %s\n",
            e.text);
        result = false;
    }

    json_decref(a);
    return result;
}

void config_init(void)
{
    json_error_t t;
    json_t *cfg = json_load_file("./winhide.json", 0, &t);
    bool result = true;

    if (cfg) {
        result = config_load(cfg);
        if (!result)
            init_defaults();
    } else {
        init_defaults();
        result = config_create();
    }

    if (!result)
        printf("Config loading failed! Using defaults\n");
    json_decref(cfg);
}

void config_close(void)
{
    target_t *current = config.first, *next;
    while (current) {
        next = current->next;
        free(current->text);
        free(current);
        current = next;
    }
}