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

#include <obs-module.h>
#include <netlib.h>
#include "source.hpp"

#define S_PLUGIN_ID			"winhide"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(S_PLUGIN_ID, "en-US")

bool obs_module_load()
{
    if (netlib_init() < 0) {
        blog(LOG_ERROR, "Couldn't initialize network library.");
        return false;
    } else {
        blog(LOG_INFO, "[winhide] Netlib v%d.%d.%d initialized",
             NETLIB_MAJOR_VERSION, NETLIB_MINOR_VERSION, NETLIB_PATHLEVEL);
    }
    source::install();
    return true;
}

void obs_module_unload()
{
    netlib_quit();
}
