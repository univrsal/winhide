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

#include "source.hpp"
#include "server.hpp"

#define S_ID		"winhide-source"
#define S_COLOR		"color"
#define S_WIDTH		"width"
#define S_HEIGHT	"height"
#define S_PORT		"port"

#define T_(t)		obs_module_text(t)
#define T_ID		T_("Winhide.Source")
#define T_COLOR		T_("WinHide.Color")
#define T_WIDTH		T_("WinHide.Width")
#define T_HEIGHT	T_("WinHide.Height")
#define T_PORT		T_("WinHide.Port")

namespace source {

    winhide::winhide(obs_source_t *source, obs_data_t *settings)
        : m_source(source), m_settings(settings)
    {
        update(settings);
        m_server = new network::server(m_port);
    }

    winhide::~winhide()
    {
        delete m_server;
    }

    void winhide::update(obs_data_t *settings)
    {
        m_port = obs_data_get_int(settings, S_PORT);
        m_cx = obs_data_get_int(settings, S_WIDTH);
        m_cy = obs_data_get_int(settings, S_HEIGHT);
        m_color = obs_data_get_int(settings, S_COLOR);
    }

    void winhide::tick(float seconds)
    {
        if (m_server->started()) {
            m_server->tick(m_windows, m_mutex);
        }
    }

    void winhide::render(gs_effect_t *effect)
    {
        m_mutex.lock();

		gs_effect_t *solid = obs_get_base_effect(OBS_EFFECT_SOLID);
		gs_eparam_t *color = gs_effect_get_param_by_name(solid, "color");
		gs_technique_t *tech = gs_effect_get_technique(solid, "Solid");

		struct vec4 colorVal;
		vec4_from_rgba(&colorVal, m_color);
		gs_effect_set_vec4(color, &colorVal);

		gs_technique_begin(tech);
		gs_technique_begin_pass(tech, 0);

		for (const auto& r : m_windows) {
			gs_matrix_push();
			gs_matrix_translate3f(r.x, r.y, 0);
			gs_draw_sprite(nullptr, 0, r.w, r.h);
			gs_matrix_pop();
		}

		gs_technique_end_pass(tech);
		gs_technique_end(tech);

        m_mutex.unlock();
    }


	obs_properties_t *get_properties(void *data)
	{
		UNUSED_PARAMETER(data);
		obs_properties_t *props = obs_properties_create();
		obs_properties_add_color(props, S_COLOR, T_COLOR);
		obs_properties_add_int(props, S_WIDTH, T_WIDTH, 0, 4096, 1);
		obs_properties_add_int(props, S_HEIGHT, T_HEIGHT, 0, 4069, 1);
		obs_properties_add_int(props, S_PORT, T_PORT, 1024, 65000, 1);
		return props;
	}

	void install()
	{
		obs_source_info si = {};
		si.id = S_ID;
		si.type = OBS_SOURCE_TYPE_INPUT;
		si.output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_CUSTOM_DRAW;
		si.get_properties = get_properties;
		si.get_name = [](void *) { return T_ID; };

		si.create = [](obs_data_t *s, obs_source_t *src) {
			return static_cast<void *>(new winhide(src, s));
		};
		si.destroy = [](void *data) { delete reinterpret_cast<winhide *>(data); };
		si.get_width = [](void *data) { return reinterpret_cast<winhide *>(data)->get_width(); };
		si.get_height = [](void *data) { return reinterpret_cast<winhide *>(data)->get_height(); };

		si.get_defaults = [](obs_data_t *settings) {
			obs_data_set_default_int(settings, S_PORT, 16899);
			obs_data_set_default_int(settings, S_WIDTH, 1280);
			obs_data_set_default_int(settings, S_HEIGHT, 720);
			obs_data_set_default_int(settings, S_COLOR, 0xffffffff);
		};

		si.update = [](void *data, obs_data_t *settings) {
			reinterpret_cast<winhide *>(data)->update(settings);
		};
		si.video_tick = [](void *data, float seconds) {
			reinterpret_cast<winhide *>(data)->tick(seconds);
		};
		si.video_render = [](void *data, gs_effect_t *effect) {
			reinterpret_cast<winhide *>(data)->render(effect);
		};

		obs_register_source(&si);
	}
}
