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

#define S_COLOR		"color"
#define S_WIDTH		"width"
#define S_HEIGHT	"height"
#define S_PORT		"port"

#define T_(t)		obs_module_text(t)
#define T_COLOR		T_("WinHide.Color")
#define T_WIDTH		T_("WinHide.Width")
#define T_HEIGHT	T_("WinHide.Height")
#define T_PORT		T_("WinHide.Port")

namespace source {

    winhide::winhide(obs_source_t *source, obs_data_t *settings)
        : m_source(source), m_settings(settings)
    {
        update(settings);
    }

    winhide::~winhide()
    {

    }

    void winhide::update(obs_data_t *settings)
    {

    }

    void winhide::tick(float seconds)
    {

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


	static obs_properties_t *get_properties(void *data)
	{
		UNUSED_PARAMETER(data);
		obs_properties_t *props = obs_properties_create();
		obs_properties_add_color(props, S_COLOR, T_COLOR);
		obs_properties_add_int(props, S_WIDTH, T_WIDTH, 0, 4096, 1);
		obs_properties_add_int(props, S_HEIGHT, T_HEIGHT, 0, 4069, 1);
		obs_properties_add_int(props, S_PORT, T_PORT, 1024, 65000, 1);
	}
}
