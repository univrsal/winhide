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

#pragma once
#include <obs-module.h>

struct rect {
	uint16_t x, y, w, h;
};

#define debug(format, ...) \
	blog(LOG_DEBUG, "[winhide] " format, ##__VA_ARGS__)
#define info(format, ...) \
	blog(LOG_INFO, "[winhide] " format, ##__VA_ARGS__)
#define warn(format, ...) \
	blog(LOG_WARNING, "[winhide] " format, ##__VA_ARGS__)

#define debugs(format, ...) \
	blog(LOG_DEBUG, "[winhide: %s] " format, obs_source_get_name(m_source), ##__VA_ARGS__)
#define infos(format, ...) \
	blog(LOG_INFO, "[winhide: %s] " format, obs_source_get_name(m_source), ##__VA_ARGS__)
#define warns(format, ...) \
	blog(LOG_WARNING, "[winhide: %s] " format, obs_source_get_name(m_source), ##__VA_ARGS__)

#define ip_shift(ip) \
	(ip >> 0) & 0xFF, (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 25)
