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
#include "util.hpp"
#include <mutex>
#include <netlib.h>
#include <vector>

namespace network {

class server {
    ip_address m_ip;
    tcp_socket m_server_socket = nullptr;
    tcp_socket m_client_socket = nullptr;
    netlib_socket_set m_sockets = nullptr;
    netlib_byte_buf* m_buf;
    bool m_started = false;

    void init();
    bool connect_client();
    void receive_windows(std::vector<rect>& r);

public:
    server(uint16_t port);
    ~server();

    void tick(std::vector<rect>& windows, std::mutex& m);
    bool started() const { return m_started; }
    void disconnect_client();
};
}
