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
#include <vector>
#include <mutex>
#include "util.hpp"

namespace network { class server; }

namespace source {

class winhide {
    uint32_t m_color = 0xffffffff;
    uint32_t m_cx = 1280, m_cy = 720;
    uint16_t m_port = 16899;

    std::vector<rect> m_windows;
    std::mutex m_mutex;
    network::server *m_server;
    obs_data_t *m_settings;
    obs_source_t *m_source;
public:
    winhide(obs_source_t *src, obs_data *settings);
    ~winhide();

    inline void update(obs_data_t *settings);
    inline void tick(float seconds);
    inline void render(gs_effect_t *effect);

    uint32_t get_width() const { return m_cx; }
    uint32_t get_height() const { return m_cy; }
};

extern obs_properties_t *get_properties(void *data);
extern void install();

}
